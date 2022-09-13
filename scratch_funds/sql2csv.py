import mysql.connector
import xlsxwriter

workbook = xlsxwriter.Workbook('company_funds.xlsx')

dataBase = mysql.connector.connect(
	host ="localhost",
	user ="marcus",
	passwd ="200718",
	database = "funds"
)

cursorObject = dataBase.cursor()

# Retrieve data from company table and store in sheet 1.
worksheet = workbook.add_worksheet()

worksheet.write_string(0, 0, "name")
worksheet.write_string(0, 1, "location")
worksheet.write_string(0, 2, "scale")
worksheet.write_string(0, 3, "funds_number")
worksheet.write_string(0, 4, "manage_number")
worksheet.write_string(0, 5, "time")
worksheet.write_string(0, 6, "bondfunds_prop")
worksheet.write_string(0, 7, "stock_scale")
worksheet.write_string(0, 8, "mix_scale")
worksheet.write_string(0, 9, "bond_scale")

query = "SELECT * FROM company"
cursorObject.execute(query)
company_result = cursorObject.fetchall()

for index, company_info in enumerate(company_result):
	col_index = 0
	for info in company_info:
		worksheet.write(index + 1, col_index, info)
		col_index += 1

# Retrieve data from stocks table and store in sheet 2.
worksheet = workbook.add_worksheet()

worksheet.write_string(0, 0, "company_id")
worksheet.write_string(0, 1, "stock_name")
worksheet.write_string(0, 2, "stock_value")

query = "SELECT * FROM stocks"
cursorObject.execute(query)
stocks_result = cursorObject.fetchall()

for index, stock_info in enumerate(stocks_result):
	col_index = 0
	for info in stock_info:
		worksheet.write(index + 1, col_index, info)
		col_index += 1

# Retrieve data from mmfunds table and store in sheet 3.
worksheet = workbook.add_worksheet()

worksheet.write_string(0, 0, "company_id")
worksheet.write_string(0, 1, "fund_name")
worksheet.write_string(0, 2, "fund_value")

query = "SELECT * FROM mmfunds"
cursorObject.execute(query)
mmfunds_result = cursorObject.fetchall()

for index, mmfunds_info in enumerate(mmfunds_result):
	col_index = 0
	for info in mmfunds_info:
		worksheet.write(index + 1, col_index, info)
		col_index += 1

# Retrieve data from mmfunds table and store in sheet 4.
worksheet = workbook.add_worksheet()

worksheet.write_string(0, 0, "company_id")
worksheet.write_string(0, 1, "fund_name")
worksheet.write_string(0, 2, "fund_value")

query = "SELECT * FROM eqfunds"
cursorObject.execute(query)
eqfunds_result = cursorObject.fetchall()

for index, eqfunds_info in enumerate(eqfunds_result):
	col_index = 0
	for info in eqfunds_info:
		worksheet.write(index + 1, col_index, info)
		col_index += 1


# disconnecting from server
cursorObject.close()
dataBase.close()

workbook.close()
