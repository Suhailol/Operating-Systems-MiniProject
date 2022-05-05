x = input("Enter an integer")

#make sure the entered value is an integer

if x.isdigit() == false:
	x = input("Enter an integer")

print(int(x)+5)