from re import S
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.support.events import EventFiringWebDriver, AbstractEventListener

import mysql.connector

import pandas as pd
import time

# Get raw html codes without opening the actual browser.

# chrome_driver_binary = "/home/shensong/Desktop/Orient Futures/chromedriver"
chrome_driver_binary = "./chromedriver"
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

# Create tables

companyRecord = """CREATE TABLE IF NOT EXISTS company (
                   name  VARCHAR(20) NOT NULL,
                   location VARCHAR(50),
                   scale VARCHAR(20) NOT NULL,
                   funds_no INT,
                   manage_no INT,
                   time VARCHAR(20),
				   bondfunds_prop FLOAT,
				   stock_scale VARCHAR(20),
				   mix_scale VARCHAR(20),
				   bond_scale VARCHAR(20)
                  )"""
cursorObject.execute(companyRecord)

stockRecord = """CREATE TABLE IF NOT EXISTS stocks (
                   company_id INT NOT NULL,
				   stock_name VARCHAR(20),
				   stock_value VARCHAR(20)
                  )"""
cursorObject.execute(stockRecord)

fundRecord = """CREATE TABLE IF NOT EXISTS funds (
                   company_id INT NOT NULL,
				   fund_name VARCHAR(50),
				   fund_type VARCHAR(20)
                  )"""
cursorObject.execute(fundRecord)

mmFundRecord = """CREATE TABLE IF NOT EXISTS mmfunds (
                   company_id INT NOT NULL,
				   fund_name VARCHAR(50),
				   fund_type VARCHAR(20)
                  )"""
cursorObject.execute(mmFundRecord)

eqFundRecord = """CREATE TABLE IF NOT EXISTS eqfunds (
                   company_id INT NOT NULL,
				   fund_name VARCHAR(50),
				   fund_type VARCHAR(20)
                  )"""
cursorObject.execute(eqFundRecord)

i = 1
for company_sample in company_web:
	if i % 20 == 0:
		print(i)

	# The last 10 comapnies have too little information.
	if i == 190:
		break
	# start = time.time()

	driver.get(company_sample)

	# end = time.time()
	# print("scratch time: " + str(end - start))

	# Scratch stock data
	stock_name = driver.find_elements(By.CSS_SELECTOR, "#js-gpcc-body tr td:nth-child(3) a")
	stock_value = driver.find_elements(By.CSS_SELECTOR, "#js-gpcc-body tr td:last-child")
	for j in range(len(stock_value)):
		insert = "INSERT INTO stocks (company_id, stock_name, stock_value)\
			VALUES (%s, %s, %s)"
		val = (i, stock_name[j].text, stock_value[j].text)
		cursorObject.execute(insert, val)

	# Scratch all funds owned by the company.
	allFunds = driver.find_elements(By.CSS_SELECTOR, ".common-sort-table .fund-name-code a:first-child" )
	allTypes = driver.find_elements(By.CSS_SELECTOR, ".common-sort-table tr td:nth-child(3)" )
	for j in range(len(allFunds)):
		insert = "INSERT INTO funds (company_id, fund_name, fund_type)\
			VALUES (%s, %s, %s)"
		val = (i, allFunds[j].text, allTypes[j].text)
		cursorObject.execute(insert, val)

	# Find the proportion of number of bond funds.
	query = """SELECT * FROM funds WHERE (fund_type = '债券型-混合债' OR fund_type = '债券型-长债' OR fund_type = '混合型-偏债' OR fund_type = '债券型-中短债') AND company_id = %s""" % i
	cursorObject.execute(query)
	bondfunds_lst = cursorObject.fetchall()
	bondfunds_prop = 0
	if len(allFunds) != 0:
		bondfunds_prop = len(bondfunds_lst) / len(allFunds) * 100
	
	# Scratch basic information
	company_name = driver.find_element(By.CSS_SELECTOR, ".ttjj-panel-main-title").text
	company_scale = driver.find_element(By.CSS_SELECTOR, ".fund-info li:first-child label").text
	fund_num = driver.find_element(By.CSS_SELECTOR, ".fund-info li:nth-child(2) label a").text
	manage_num = driver.find_element(By.CSS_SELECTOR, ".fund-info li:nth-child(3) label a").text
	company_date = driver.find_element(By.CSS_SELECTOR, ".date label").text
	company_loc = driver.find_element(By.CSS_SELECTOR, ".gs-info label").text

	# Find out scales for different fund types in the company: stock / mix / bond.
	scale_tags = driver.find_elements(By.CSS_SELECTOR, "#sygmTags li")
	scale_tags = scale_tags[:3]

	scale_texts = []
	for scale_tag in scale_tags:
		scale_tag.click()
		time.sleep(1)
		scale_texts.append(driver.find_element(By.CSS_SELECTOR, "#gmTableWrap tr:nth-child(2) td:nth-child(2)").text)

	# Scale tags can be less than 3, use 0 to complement.
	while len(scale_texts) < 3:
		scale_texts.append("0")

	# stock_scale = driver.find_element(By.CSS_SELECTOR, "#gmTableWrap tr:nth-child(2) td:nth-child(2)").text
	insert = "INSERT INTO company (name, location, scale, funds_no, manage_no, time, bondfunds_prop, stock_scale, mix_scale, bond_scale)\
		VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s)"
	val = (company_name, company_loc, company_scale, fund_num, manage_num, company_date, bondfunds_prop, scale_texts[0], scale_texts[1], scale_texts[2])
	cursorObject.execute(insert, val)

	i += 1

# Select money market funds and store in the mmfunds table.
query = """SELECT * FROM funds WHERE fund_type = '货币型'"""
cursorObject.execute(query)
mmfunds_lst = cursorObject.fetchall()
for mmfund in mmfunds_lst:
	insert = "INSERT INTO mmfunds (company_id, fund_name, fund_type)\
			VALUES (%s, %s, %s)"
	cursorObject.execute(insert, mmfund)

# Select money market funds and store in the eqfunds table.
query = """SELECT * FROM funds WHERE fund_type = '指数型-股票' OR fund_type = '混合型-偏股' OR fund_type = '股票型'"""
cursorObject.execute(query)
eqfunds_lst = cursorObject.fetchall()
for eqfund in eqfunds_lst:
	insert = "INSERT INTO eqfunds (company_id, fund_name, fund_type)\
			VALUES (%s, %s, %s)"
	cursorObject.execute(insert, eqfund)

query = """SELECT * FROM company"""
cursorObject.execute(query)
eqfunds_lst = cursorObject.fetchall()
# for each in eqfunds_lst:
# 	print(each)

dataBase.commit()

# disconnecting from server
cursorObject.close()
dataBase.close()
