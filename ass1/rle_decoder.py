# usage: just cp-p the pattern into the rleFile.py file and fill in appropriate x, y measurements (without the height and width params)

import re
from rleFile import rle, x, y

output = input("Enter s for MIPS .byte output, c for C #define output: ")

if (x < y):
	x = y
else:
	y = x

def cleanAndBreakdown(rle): # takes rle as a string
	rle = rle.replace('o', 'a')
	rle = rle.replace('b', 'd')
	rle = rle.replace('!', '')
	rle = rle.replace('\n', '')
	list = rle.split('$')
	return list

row_list = cleanAndBreakdown(rle)

length = len(row_list)
for i in range(0, length):

	l = len(row_list[i])
	digit = ""
	decrypted_row = ""
	for j in range(0, l): # for each character in each line
		if (row_list[i][j] .isdigit()):
			digit += row_list[i][j]
		else:
			#print(digit)
			num_symbols = 1
			if (digit != ""):
				num_symbols = int(digit)
			digit = ""
			if (row_list[i][j] == 'a'):
				for c in range(0,num_symbols):
					decrypted_row += '1'
			elif (row_list[i][j] == 'd'):
				for c in range(0,num_symbols):
					decrypted_row += '0'
	dr_len = len(decrypted_row)
	if (dr_len < x):
		padding_length = x - dr_len
		for p in range(0, padding_length):
			decrypted_row += '0'

	#insert commas to separate all 1's and 0's except after last element
	decrypted_row = ','.join(decrypted_row[i:i+1] for i in range(0, len(decrypted_row), 1))
	if (output == 's'):
		print(".byte "+decrypted_row)
	elif (output == 'c'):
		print("{"+decrypted_row+"},")


#scale table in y direction
decrypted_row = ""
if (length < y):
	for x_len in range(0, x):
		decrypted_row += '0'
	decrypted_row = ','.join(decrypted_row[i:i+1] for i in range(0, len(decrypted_row), 1))
	for y_scaler in range(length, y):
		if (output == 's'):
			print(".byte "+decrypted_row)
		elif (output == 'c'):
			print("{"+decrypted_row+"},")


			





	# seperated = re.match(r"([a-zA-Z]+)([0-9]+)",row_list[i])
	# print(seperated.group[0])
	# print(row_list[i])
	# m = re.match(r"([a-zA-Z]+)([0-9]+)",s)
	# print m.group(0)
	# print m.group(1)
	# print m.group(2)