#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#define OUTPUT_RECORD_LENGTH 32

int CharToHex(char* input, int length)
{
	int result = 0;

	int i;
	for (i = 0; i < length; i++)
	{
		char c = input[length-i -1];
		int val = 0;

		if (c & 32)
			val = c & 0x0F;
		if (c & 64)
			val = (c & 0x0F) + 9;

		val  = val << 4 * i;
		result += val;
	}
	return result;
}
void HexToChar(int input, char* output, int length)
{
	int i;
	for (i = 0; i < length; i++)
	{
		int val = (input >> 4 * i) & 0xF;
		if (val < 10)
			val += 48;
		else
			val += 55;
		output[length - i - 1] = (char)val;
	}
}

void CharArrayToVector(char* input, int length, std::vector<char>* target)
{
	int i;
	for (i = 0; i < length; i++)
	{
		target->push_back(input[i]);
	}

}


struct record {
	int dataLength;
	int adress;
	int recordType; // Can be 0 to 5
	int checksm;

	std::vector<char> data;

};


using namespace std;
int main(int argc, char* argv[])
{

	string fileName(argv[1]);
	int stringSearch = 0;
	for (stringSearch = 0; stringSearch < fileName.size(); stringSearch++)
	{
		if (fileName.c_str()[stringSearch] == '.')
		{
			fileName.erase(stringSearch);	//Erase the .hex from the filename so it can be appended for the output file
		}

	}
	string outputName = fileName + "_trimmed.hex";
	fileName += ".hex";

	ifstream myFile;
	myFile.open(fileName.c_str(), ios::binary);

	if (!myFile.is_open())
	{
		cout << "File could not be opened. Is it in the same folder as the application?";
		getchar();
		return 0;

	}
	vector<record> inputRecords;
	vector<record> outputRecords;
	bool done = false;

	char lengthC[2] = { 0 };
	char adressC[4] = { 0 };
	char recTypeC[2] = { 0 };
	
	int outputLength = 0;
	while (!done)
	{
		char dataBuffer[256] = { 0 };
		int dataLength = 0;
		int adress = 0;
		int recType = 0;
		record newRecord;

		//Push out the : character
		myFile.get();

		//Read the length of the data and convert it to a number
		myFile.read(lengthC, 2);
		dataLength = CharToHex(lengthC, 2)*2;

		//Read the adress and convert it to an integer
		myFile.read(adressC, 4);
		adress = CharToHex(adressC, 4);

		//Get the rec type 
		myFile.read(recTypeC, 2);
		recType = CharToHex(recTypeC, 2);

		//Read in the data
		myFile.read(dataBuffer, dataLength);
		
		//Push out the check sum characters
		myFile.get();
		myFile.get();

		//push out newline chars
		myFile.get();
		myFile.get();

		newRecord.adress = adress;
		newRecord.dataLength = dataLength;
		newRecord.recordType = recType;
		newRecord.checksm = 0;

		CharArrayToVector(dataBuffer, dataLength, &newRecord.data);
		inputRecords.push_back(newRecord);
		if (recType == 1)
		{
			done = true;
			outputLength = OUTPUT_RECORD_LENGTH + inputRecords.at(inputRecords.size() - 2).adress +OUTPUT_RECORD_LENGTH*((inputRecords.at(inputRecords.size() - 2).dataLength / 2) / OUTPUT_RECORD_LENGTH);
			myFile.close();
		}
	}
	int i;
	for (i = 0; i < outputLength; i += OUTPUT_RECORD_LENGTH)
	{
		record newRecord;
		newRecord.adress = i;
		newRecord.recordType = 0;
		newRecord.checksm = 0;
		newRecord.dataLength = OUTPUT_RECORD_LENGTH*2;
		int j;
		for (j = 0; j < OUTPUT_RECORD_LENGTH*2; j++)
		{
			newRecord.data.push_back('F');
		}
		outputRecords.push_back(newRecord);
	}




	for each (record rec in inputRecords)
	{
		int startingAdress = 0;
		int offset = 0;
		if (rec.adress % OUTPUT_RECORD_LENGTH == 0) //Divisible by output length
			startingAdress = rec.adress;
		else
		{
			startingAdress = rec.adress - (rec.adress % OUTPUT_RECORD_LENGTH);
			offset = 2*(rec.adress - startingAdress);
		}
		int i;
		for (i = 0; i < rec.dataLength; i++)
		{
			int index = (startingAdress / OUTPUT_RECORD_LENGTH) + (((i + offset) / (OUTPUT_RECORD_LENGTH*2)));
			outputRecords.at(index).data.at((i+ offset) % (2*OUTPUT_RECORD_LENGTH)) = rec.data.at(i);
		}

		

	}
	ofstream output;

	output.open(outputName.c_str(), ios::binary | ios::out);

	if (!output.is_open())
	{
		cout << "Could not open output";
		getchar();

	}
	for each (record rec in outputRecords)
	{
		char dataLength[2];
		char adress[4];
		char recType[2];
		char checkSum[2];
		HexToChar(rec.dataLength/2, dataLength, 2);
		HexToChar(rec.adress, adress, 4);
		HexToChar(rec.recordType, recType, 2);
		

		output.put(':');
		output.write(dataLength, 2);
		output.write(adress, 4);
		output.write(recType, 2);

		unsigned char cc = 0;
		cc += rec.dataLength / 2;
		cc += rec.adress & 0x00FF;
		cc += (rec.adress & 0xFF00) >> 8;
		cc += rec.recordType;

		int flip = 1;
		for each(char c in rec.data)
		{
			output.put(c);

			cc += CharToHex(&c, 1) << flip*4;

			flip = (flip + 1) % 2;

		}
		cc = ~cc + 1;
		HexToChar(cc, checkSum, 2);
		output.write(checkSum, 2);
		output.write("\r\n", 2);
	}
	output.write(":00000001FF",11);
	output.close();
	return 0;
}