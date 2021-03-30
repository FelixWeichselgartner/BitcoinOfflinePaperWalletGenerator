git clone https://github.com/PaulGregor/Bitcoin-Keys-Generator.git
echo "overwrite line 55 in generators.py:"
echo "private_key = hexlify(PACKER.pack(number0, number1, number2, number3)).decode(\"utf-8\")"
echo "becomes:"
echo "private_key = '<enter private key here>'"
