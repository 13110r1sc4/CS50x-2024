from flask import Flask, render_template, request, redirect, url_for
import yfinance as yf
import matplotlib.pyplot as plt
import os
import matplotlib
import requests
import pandas as pd
import numpy as np
from datetime import datetime, timedelta, timezone
from scipy.stats import norm

# Agg backend for matplotlib
matplotlib.use('Agg')

app = Flask(__name__)

# Deribit API base URL
base_url = "https://www.deribit.com/api/v2/public/"

def cn_put(T, sigma, kT):
    s = sigma * np.sqrt(T)
    d = (np.log(kT) + 0.5 * s * s) / s
    return norm.cdf(d)

def an_put(T, sigma, kT):
    s = sigma * np.sqrt(T)
    d = (np.log(kT) + 0.5 * s * s) / s
    return norm.cdf(d - s)

def FwEuroPut(T, vSigma, vKt):
    return (vKt * cn_put(T, vSigma, vKt) - an_put(T, vSigma, vKt))

def impVolFromMCPut(vPrice, T, vKt):
    scalar = isinstance(vKt, float)
    if scalar: 
        vKt = np.array([vKt])

    vSl = np.zeros(vKt.shape[0])
    vPl = np.maximum(vKt - 1., 0.0)  # call

    vSh = np.ones(vKt.shape[0])
    while True:
        vPm = FwEuroPut(T, vSh, vKt)
        if (vPm > vPrice).all(): 
            break
        vSh = 2 * vSh

    eps = 1.e-08
    d = vSh[0] - vSl[0]
    N = 2 + int(np.log(d / eps) / np.log(2))

    for n in range(N):
        vSm = 0.5 * (vSh + vSl)
        vPm = FwEuroPut(T, vSm, vKt)
        mask = vPm > vPrice
        vSh[mask] = vSm[mask]
        vSl[~mask] = vSm[~mask]
    
    if scalar: 
        return 0.5 * (vSh + vSl)[0]
    return 0.5 * (vSh + vSl)

# get the next Friday from the current date
def get_next_friday():
    today = datetime.now(timezone.utc).date()
    days_ahead = (4 - today.weekday() + 7) % 7
    if days_ahead == 0:
        days_ahead = 7
    return today + timedelta(days=days_ahead)

# convert expiration timestamp to a readable date
def convert_timestamp_to_date(timestamp_ms):
    return datetime.fromtimestamp(timestamp_ms / 1000, timezone.utc)

def get_btc_spot_price():
    ticker_url = f"{base_url}ticker?instrument_name=BTC-PERPETUAL"
    response = requests.get(ticker_url)
    data = response.json()
    
    if 'result' in data and data['result']:
        return data['result']['last_price']
    
    return None

def get_option_instruments_for_expiration(expiration_date, currency='BTC'):
    endpoint = f"{base_url}get_instruments?currency={currency}&kind=option"
    response = requests.get(endpoint)
    data = response.json()

    instruments = []
    for instrument in data['result']:
        instrument_expiration = datetime.fromtimestamp(instrument['expiration_timestamp'] / 1000, timezone.utc).date()
        if instrument_expiration == expiration_date and instrument['option_type'] == 'put':
            instruments.append(instrument)

    return instruments

def get_last_trades_for_strikes(instruments):
    last_prices = []
    
    for instrument in instruments:
        instrument_name = instrument['instrument_name']
        ticker_url = f"{base_url}ticker?instrument_name={instrument_name}"
        response = requests.get(ticker_url)
        price_data = response.json()
        
        if 'result' in price_data and price_data['result']:
            last_price = price_data['result']['last_price']
            expiration_timestamp = instrument['expiration_timestamp']
            expiration = convert_timestamp_to_date(expiration_timestamp)
            last_prices.append({
                'instrument': instrument_name,
                'expiration': expiration,
                'strike': instrument['strike'],
                'option_type': instrument['option_type'],
                'last_price': last_price
            })
    
    return last_prices

# -----------------------------------------------------------------------
# -----------------------------------------------------------------------


@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        ticker = request.form['ticker']
        period = request.form['period'] 
        return redirect(url_for('show_graph', ticker=ticker, period=period))
    return render_template('index.html')


@app.route('/graph/<ticker>/<period>', methods=['GET'])
def show_graph(ticker, period):
    try:
        # Download data for the selected ticker with the specified period
        stock_data = yf.download(ticker, period=period, interval='1d')

        plt.figure(figsize=(10, 5))
        plt.plot(stock_data.index, stock_data['Close'], label=f'{ticker} Close Price')
        plt.xlabel('Date')
        plt.ylabel('Price (USD)')
        plt.title(f'{ticker} Price Over Last {period}')
        plt.legend()

        # Save the plot as a PNG file in the static folder
        img_path = os.path.join('static', f'{ticker}.png')
        plt.savefig(img_path)
        plt.close()

        return render_template('graph.html', ticker=ticker, img_path=img_path)

    except Exception as e:
        return f"Error: {str(e)}"


@app.route('/volatility', methods=['GET', 'POST'])
def volatility():
    vol_data = {}

    if request.method == 'POST':

        asset = request.form['asset']
        risk_free_rate = float(request.form['risk_free_rate']) / 100 
        next_friday = get_next_friday()
        instruments = get_option_instruments_for_expiration(next_friday, currency=asset)
        spot_price = get_btc_spot_price()

        if spot_price is None:
            return "Error: Unable to retrieve spot price."

        # Get last traded prices for strikes
        if instruments:
            last_prices = get_last_trades_for_strikes(instruments)
            strikes = []
            volatilities = []

            for option in last_prices:
                strike = option['strike']
                vPrice = option['last_price']
                if vPrice is None:
                    continue
                T = (option['expiration'].date() - datetime.now(timezone.utc).date()).days / 365  # Calculate T in years
                So = spot_price
                Fw = So*np.exp(risk_free_rate*T)
                kT = strike/Fw   
                
                
                vol = impVolFromMCPut(vPrice, T, kT)
                

                if vol is not None:
                    vol_data[strike] = vol
                    strikes.append(strike)
                    volatilities.append(vol)

            # Plot the implied volatility
            plt.figure(figsize=(10, 5))
            plt.plot(strikes, volatilities, marker='o', linestyle='-', color='b')
            plt.xlabel('Strike Price')
            plt.ylabel('Implied Volatility')
            plt.title(f'NextFriday IV for {asset}')
            plt.grid(True)
            
            img_path = os.path.join('static', f'{asset}_volatility.png')
            plt.savefig(img_path)
            plt.close()

            return render_template('volatility.html', img_path=f'{asset}_volatility.png')

    return render_template('volatility.html')


if __name__ == '__main__':
    app.run(debug=True, port=5000)
