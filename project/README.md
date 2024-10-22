# TRACKER
#### Video Demo:  <URL https://youtu.be/jMrzhji5jas>
#### Description:
Tracker is a web based application that displays financial data. In particular, two features are added as part of the app: the first one retrieves stock price data from yahoo finance API and displays a graph (periods available: 1mo, 6mo, 1y); the second one downloads options prices for BTC with expiry set to 'next friday' for different strikes through Deribit API and displays the related graph for the computed implied volatility, as implied by BS model.


In the project folder, one can find:

- **app.py**: The application leverages external libraries such as yfinance for stock data retrieval, matplotlib for plotting, requests for API calls to the Deribit exchange for options data, datetime, numpy, pandas, scipy, flask. The app.py file is the core of the Flask application, responsible for defining routes, handling requests, and processing data. The application structure is organized into several functions, each serving a specific purpose:

    1. At the start of the file, necessary libraries are imported, including Flask components, data handling libraries and statistical functions. The Agg backend for matplotlib is set to enable non-GUI plotting, which is suitable for web applications running on servers.

    2. The application contains several utility functions:
        - cn_put and an_put: These functions compute the cumulative normal distribution for option pricing. They are used to determine the probability that the underlying asset's price will be below a certain level.
        - FwEuroPut: This function calculates the forward price of a European put option using the Black-Scholes model.
        - impVolFromMCPut: This function computes implied volatility from market prices using a binary search algorithm, iteratively adjusting the volatility estimate until the theoretical price converges with the market price.
        - get_next_friday: This function determines the next Friday's date from the current date, crucial for identifying option expiration dates.
        - convert_timestamp_to_date: Converts timestamps from milliseconds to human-readable date formats.

    3. Several functions are dedicated to retrieving market data:
        - get_btc_spot_price: This function fetches the current spot price of Bitcoin from the Deribit API. It constructs the API request URL and parses the JSON response to extract the last price.
        - get_option_instruments_for_expiration: This function retrieves available option instruments for a specified expiration date. It filters the results to include only put options.
        - get_last_trades_for_strikes: This function retrieves the last traded prices for the specified strike prices of options. It queries the Deribit API for each option instrument and compiles the results into a list.

    4. The application defines several routes to handle user requests:
        - index: This route renders a form where users can input a stock ticker and select a period (1 month, 6 months, or 1 year) for analysis. Upon form submission, it redirects to the show_graph route.
        - graph: This route retrieves historical stock price data using the yfinance library, generates a price graph using matplotlib, saves it as a PNG file in the static directory, and renders the graph.html template to display the graph.
        - volatility: This route provides a form for users to calculate implied volatility. Users select an asset (BTC or ETH) and input a risk-free rate. The server processes this information, retrieves relevant option instruments, calculates implied volatilities for the strikes, plots the results, and renders the volatility.html template to display the results.

- the **static** folder: The CSS file styles the application, ensuring a visually appealing layout. It includes styles for the body, headers, navigation, and form elements to create a user-friendly interface.

- the **templates** folder: The application employs Jinja2 templating to dynamically render HTML pages:
    - **layout.html**: This template establishes a consistent layout for the application, including a navigation menu and a footer. It provides a clean structure for other templates to extend.
    - **index.html**: This template serves as the homepage, where users can input a stock ticker and select a period for analysis. It extends the layout.html template, maintaining a consistent design across the application.
    - **graph.html**: This template displays the generated stock price graph. It dynamically inserts the ticker and image path, allowing users to visualize the stock's performance over the selected period.
    - **volatility.html**: This template provides a form for calculating implied volatility. Upon submission, it displays a table of calculated volatilities along with a graph if available.
