from re import S
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By

import mysql.connector

import pandas as pd
import time

# Get raw html codes without opening the actual browser.

chrome_driver_binary = "/home/shensong/Desktop/Orient Futures/chromedriver"
options = Options()
options.add_argument("headless")
options.headless = True
prefs = {"profile.managed_default_content_settings.images": 2}
options.add_experimental_option("prefs", prefs)
driver = webdriver.Chrome(chrome_driver_binary, options=options)
driver.get('https://fund.eastmoney.com/company')

# Go into detailed page of each fun.
company_web = []
company_list = driver.find_elements(By.CSS_SELECTOR, ".menu-link a:first-child")
for company in company_list:
	company_web.append(company.get_attribute("href"))


# # Sample page
# company_sample = company_web[0]
# driver.get(company_sample)
# company_name = driver.find_element(By.CSS_SELECTOR, ".ttjj-panel-main-title")
# company_scale = driver.find_element(By.CSS_SELECTOR, ".fund-info li:first-child label")
# fund_num = driver.find_element(By.CSS_SELECTOR, ".fund-info li:nth-child(2) label a")
# manage_num = driver.find_element(By.CSS_SELECTOR, ".fund-info li:nth-child(3) label a")
# foundation_date = driver.find_element(By.CSS_SELECTOR, ".date label")
# gs_info = driver.find_element(By.CSS_SELECTOR, ".gs-info label")
# print(manage_num.text)

# Store the data in the local MySQL.
dataBase = mysql.connector.connect(
  host ="localhost",
  user ="marcus",
  passwd ="200718",
  database = "funds"
)

cursorObject = dataBase.cursor()

# Create table 
companyRecord = """CREATE TABLE IF NOT EXISTS FUNDS (
                   NAME  VARCHAR(20) NOT NULL,
                   LOCATION VARCHAR(50),
                   SCALE VARCHAR(20) NOT NULL,
                   FUNDS_NO INT,
                   MANAGE_NO INT,
                   TIME VARCHAR(20)
                  )"""
cursorObject.execute(companyRecord)

stockRecord = """CREATE TABLE IF NOT EXISTS HOLDS (
                   COMPANY_ID INT NOT NULL,
				   STOCK_NAME VARCHAR(20),
				   STOCK_VALUE VARCHAR(20)
                  )"""
cursorObject.execute(stockRecord)

i = 1
for company_sample in company_web:
	if i % 10 == 0:
		print(i)

	# The last 10 comapnies have too little information.
	if i == 190:
		break
	# start = time.time()

	driver.get(company_sample)

	company_name = driver.find_element(By.CSS_SELECTOR, ".ttjj-panel-main-title").text
	company_scale = driver.find_element(By.CSS_SELECTOR, ".fund-info li:first-child label").text
	fund_num = driver.find_element(By.CSS_SELECTOR, ".fund-info li:nth-child(2) label a").text
	manage_num = driver.find_element(By.CSS_SELECTOR, ".fund-info li:nth-child(3) label a").text
	company_date = driver.find_element(By.CSS_SELECTOR, ".date label").text
	company_loc = driver.find_element(By.CSS_SELECTOR, ".gs-info label").text

	# end = time.time()
	# print("scratch time: " + str(end - start))

	# Insert data
	insert = "INSERT INTO FUNDS (NAME, LOCATION, SCALE, FUNDS_NO, MANAGE_NO, TIME)\
		VALUES (%s, %s, %s, %s, %s, %s)"
	val = (company_name, company_loc, company_scale, fund_num, manage_num, company_date)
	cursorObject.execute(insert, val)

	stock_name = driver.find_elements(By.CSS_SELECTOR, "#js-gpcc-body tr td:nth-child(3) a")
	stock_value = driver.find_elements(By.CSS_SELECTOR, "#js-gpcc-body tr td:last-child")
	for j in range(len(stock_value)):
		insert = "INSERT INTO HOLDS (COMPANY_ID, STOCK_NAME, STOCK_VALUE)\
			VALUES (%s, %s, %s)"
		val = (i, stock_name[j].text, stock_value[j].text)
		cursorObject.execute(insert, val)

	i += 1

# # Retrieve data
# query = "SELECT * FROM HOLDS ORDER BY COMPANY_ID DESC"
# cursorObject.execute(query)

# myresult = cursorObject.fetchall()

# for each in myresult:
# 	print(each)

dataBase.commit()

# disconnecting from server
cursorObject.close()
dataBase.close()
