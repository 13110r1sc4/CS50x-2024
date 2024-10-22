import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    # Get user's cash balance
    user = db.execute("SELECT cash FROM users WHERE id = ?", session["user_id"])[0]

    # Get user's stock holdings (this assumes a 'transactions' table exists)
    stocks = db.execute("""
        SELECT symbol, SUM(shares) as total_shares
        FROM transactions
        WHERE user_id = ?
        GROUP BY symbol
        HAVING total_shares > 0
    """, session["user_id"])

    # Calculate total portfolio value (stocks + cash)
    total_value = user["cash"]
    portfolio = []
    for stock in stocks:
        # Get the current price of the stock
        quote = lookup(stock["symbol"])
        if quote:
            total = stock["total_shares"] * quote["price"]
            portfolio.append({
                "symbol": stock["symbol"],
                "shares": stock["total_shares"],
                "price": usd(quote["price"]),
                "total": usd(total)
            })
            total_value += total

    # Render the index page with the user's portfolio and cash
    return render_template("index.html", cash=usd(user["cash"]), portfolio=portfolio, total_value=usd(total_value))


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    if request.method == "POST":
        # Retrieve the stock symbol and number of shares from the form
        symbol = request.form.get("symbol")
        shares = request.form.get("shares")

        # Validate the input
        if not symbol or not shares:
            return apology("must provide symbol and shares", 400)

        try:
            shares = int(shares)  # This will raise ValueError for non-integer inputs
        except ValueError:
            return apology("shares must be a whole number", 400)

        if shares <= 0:
            return apology("must buy a positive number of shares", 400)

        # Look up the stock price and make the purchase logic here
        quote = lookup(symbol)
        if quote is None:
            return apology("invalid symbol", 400)

        price = quote["price"]
        total_cost = price * shares

        # Check if user has enough cash
        user_id = session["user_id"]
        user_cash = db.execute("SELECT cash FROM users WHERE id = ?", user_id)[0]["cash"]

        if total_cost > user_cash:
            return apology("not enough cash", 400)

        # Proceed with the purchase
        db.execute("UPDATE users SET cash = cash - ? WHERE id = ?", total_cost, user_id)
        db.execute("INSERT INTO transactions (user_id, symbol, shares, price) VALUES (?, ?, ?, ?)",
                   user_id, symbol, shares, price)

        flash("Purchased!")
        return redirect("/")

    # Render the buy page
    return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    user_id = session["user_id"]

    # Retrieve the user's transaction history from the database
    transactions_db = db.execute("SELECT * FROM transactions WHERE user_id = ?", user_id)

    # Pass transactions to the template
    return render_template("history.html", transactions=transactions_db, usd=usd)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""
    session.clear()

    if request.method == "POST":
        username = request.form.get("username")
        password = request.form.get("password")

        if not username:
            return apology("must provide username", 403)
        if not password:
            return apology("must provide password", 403)

        rows = db.execute("SELECT * FROM users WHERE username = ?", username)

        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], password):
            return apology("invalid username and/or password", 403)

        session["user_id"] = rows[0]["id"]
        return redirect("/")

    return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""
    session.clear()
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    if request.method == "POST":
        # Get the symbol from the form
        symbol = request.form.get("symbol")
        # Lookup the stock information
        quote = lookup(symbol)

        # Ensure the lookup was successful
        if quote is None:
            return apology("Invalid stock symbol", 400)

        # Render the quoted.html template with the quote information
        return render_template("quoted.html", quote=quote)
    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    if request.method == "POST":
        username = request.form.get("username")
        password = request.form.get("password")
        confirmation = request.form.get("confirmation")

        if not username:
            return apology("must provide username", 400)
        if not password:
            return apology("must provide password", 400)
        if password != confirmation:
            return apology("passwords must match", 400)

        hash_pass = generate_password_hash(password)

        try:
            user_id = db.execute(
                "INSERT INTO users (username, hash) VALUES (?, ?)", username, hash_pass)
        except:
            return apology("username already taken", 400)

        session["user_id"] = user_id
        return redirect("/")

    return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    user_id = session["user_id"]

    if request.method == "POST":
        symbol = request.form.get("symbol").upper()
        shares = int(request.form.get("shares"))

        if not symbol or shares <= 0:
            return apology("invalid input", 400)

        stock = lookup(symbol)
        if stock is None:
            return apology("invalid stock symbol", 400)

        # Check if user owns enough shares to sell
        owned_shares = db.execute("SELECT SUM(shares) AS total_shares FROM transactions WHERE user_id = ? AND symbol = ? GROUP BY symbol",
                                  user_id, symbol)[0]["total_shares"]

        if owned_shares < shares:
            return apology("not enough shares", 400)

        # Calculate the value of the sale and update user cash
        total_value = shares * stock["price"]
        db.execute("UPDATE users SET cash = cash + ? WHERE id = ?", total_value, user_id)
        db.execute("INSERT INTO transactions (user_id, symbol, shares, price) VALUES (?, ?, ?, ?)",
                   user_id, symbol, -shares, stock["price"])

        flash("Sold!")
        return redirect("/")

    else:
        # Get the user's owned stocks for dropdown selection
        stocks = db.execute(
            "SELECT symbol FROM transactions WHERE user_id = ? GROUP BY symbol", user_id)
        return render_template("sell.html", stocks=stocks)
