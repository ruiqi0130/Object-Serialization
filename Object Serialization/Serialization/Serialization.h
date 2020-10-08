#ifndef _SERIALIZE_HEADER_H_
#define _SERIALIZE_HEADER_H_
#include <sstream>   //std::stringstream
#include <vector>    //std::vector
#include <list>        //std::list
#include <set>       //std::set
#include <map>     //std::map
#include <utility>    // std::pair
#include <iterator>  //std::back_inserter
#include <string.h>  //memcpy
#include <fstream>  //std::fstream
#include <iostream>
#include <string>	//std::string
#include <cstdlib>

namespace BS{

	// define normal template function
	template<typename SerializableType>
	std::string serialize(SerializableType& a) //serialize from a string
	{
		return a.serialize();
	}

	template<typename SerializableType>
	int deserialize(std::string &str, SerializableType& a) //deserialize from a string
	{
		return a.deserialize(str);
	}


	
	//Serialize for basic type

	// for char
	template<>
	std::string serialize(char& b)
	{
		std::string ret;
		ret.append((const char*)&b, sizeof(char));
		return ret;
	}
	template<>
	int deserialize(std::string& str, char& b)
	{
		memcpy(&b, str.data(), sizeof(char));
		return sizeof(char);
	}

	// for int
	template<>
	std::string serialize(int& b)
	{
		std::string ret;
		ret.append((const char*)&b, sizeof(int));
		return ret;
	}
	template<>
	int deserialize(std::string& str, int& b)
	{
		memcpy(&b, str.data(), sizeof(int));
		return sizeof(int);
	}

	// for float
	template<>
	std::string serialize(float& b)
	{
		std::string ret;
		ret.append((const char*)&b, sizeof(float));
		return ret;
	}
	template<>
	int deserialize(std::string& str, float& b)
	{
		memcpy(&b, str.data(), sizeof(float));
		return sizeof(float);
	}

	// for double
	template<>
	std::string serialize(double& b)
	{
		std::string ret;
		ret.append((const char*)&b, sizeof(double));
		return ret;
	}
	template<>
	int deserialize(std::string& str, double& b)
	{
		memcpy(&b, str.data(), sizeof(double));
		return sizeof(double);
	}


	//Serialize for std::string (len+str.data())
	template<>
	std::string serialize(std::string& s)
	{
		int len = s.size();
		std::string ret;
		ret.append(BS::serialize(len));
		ret.append(s.data(), len);
		return ret;
	}

	template<>
	int deserialize(std::string &str, std::string& s)
	{
		int len;
		BS::deserialize(str, len);
		s = str.substr(sizeof(len), len);
		return sizeof(int) + len;
	}


	////////////////////////////////////////////
	//Serialize for custom class object
	//If your class object want to be serialized,
	//Plese derive for this base class
	///////////////////////////////////////////
	class Serializable
	{
	public:
		virtual std::string serialize() = 0; //The base class includes two virtual functions
		virtual int deserialize(std::string&) = 0;
	};




	//Define input and output stream for serialize & deserialize type
	//output stream
	class OutStream
	{
	public:

		OutStream() : os(std::ios::binary)
		{
		}

		//outstream for BasicType
		template<typename SerializableType>
		OutStream& operator<< (SerializableType& a)
		{
			std::string x = BS::serialize(a);
			os.write(x.data(), x.size());
			return *this;
		}

		//outstream for vector
		template<typename BasicType>
		OutStream& operator<< (std::vector<BasicType>& a)
		{
			int len = a.size();
			std::string x = BS::serialize(len);
			os.write(x.data(), x.size());

			for (int i = 0; i < len; ++i)
			{
				std::string item = BS::serialize(a[i]); //serialize the element stored in vector
				os.write(item.data(), item.size()); 
			}

			return *this;
		}

		//outstream for list
		template<typename BasicType>
		OutStream& operator<< (std::list<BasicType>& a)
		{
			std::vector<BasicType> temp;
			std::copy(a.begin(), a.end(), std::back_inserter(temp)); //transfer list to vector
			return this->operator<< (temp);
		}

		//outstream for set
		template<typename BasicType>
		OutStream& operator<< (std::set<BasicType>& a)
		{
			std::vector<BasicType> temp;
			std::copy(a.begin(), a.end(), std::back_inserter(temp)); //transfer set to vector
			return this->operator<< (temp);
		}

		//outstream for map
		template<typename BasicTypeA, typename BasicTypeB>
		OutStream& operator<< (std::map<BasicTypeA, BasicTypeB>& a)
		{
			std::vector<BasicTypeA> tempKey;
			std::vector<BasicTypeB> tempVal;

			typename std::map<BasicTypeA, BasicTypeB>::const_iterator it;
			for (it = a.begin(); it != a.end(); ++it) //transfer map::Key & map::Value to two vectors
			{
				tempKey.push_back(it->first);
				tempVal.push_back(it->second);
			}

			this->operator<< (tempKey);
			return this->operator<< (tempVal);
		}

		//outstream for pair
		template<typename BasicTypeA, typename BasicTypeB>
		OutStream& operator<< (std::pair<BasicTypeA, BasicTypeB>& a)
		{
			BasicTypeA tempfirst=a.first;
			BasicTypeB tempsecond=a.second;
	
			this->operator<< (tempfirst);
			return this->operator<< (tempsecond);
		}

		std::string str()
		{
			return os.str();
		}

	public:
		std::ostringstream os;
	};


	//input stream
	class InStream
	{
	public:

		InStream(std::string &s) : str(s), total(s.size())
		{}

		//instream for BasicType
		template<typename SerializableType>
		InStream& operator>> (SerializableType& a)
		{
			int ret = BS::deserialize(str, a);
			str = str.substr(ret); 
			return *this;
		}

		//instream for vector
		template<typename BasicType>
		InStream& operator>> (std::vector<BasicType>& a)
		{
			int len = 0;
			int ret = BS::deserialize(str, len);
			str = str.substr(ret);

			for (int i = 0; i < len; ++i)
			{
				BasicType item;
				int size = BS::deserialize(str, item);
				str = str.substr(size);
				a.push_back(item);
			}

			return *this;
		}

		//instream for list
		template<typename BasicType>
		InStream& operator>> (std::list<BasicType>& a)
		{
			std::vector<BasicType> temp;
			InStream& ret = this->operator>> (temp);
			if (temp.size() > 0)
			{
				std::copy(temp.begin(), temp.end(), std::back_inserter(a));
			}

			return ret;
		}

		//instream for set
		template<typename BasicType>
		InStream& operator>> (std::set<BasicType>& a)
		{
			std::vector<BasicType> temp;
			InStream& ret = this->operator>> (temp);
			if (temp.size() > 0)
			{
				for (size_t i = 0; i < temp.size(); ++i)
				{
					a.insert(temp[i]);
				}
			}

			return ret;
		}

		//instream for map
		template<typename BasicTypeA, typename BasicTypeB>
		InStream& operator>> (std::map<BasicTypeA, BasicTypeB>& a)
		{
			std::vector<BasicTypeA> tempKey;
			std::vector<BasicTypeB> tempVal;

			this->operator>> (tempKey);
			InStream& ret = this->operator>> (tempVal);

			if (tempKey.size() > 0 && tempVal.size() == tempKey.size())
			{
				for (size_t i = 0; i < tempKey.size(); ++i)
				{
					a.insert(std::make_pair(tempKey[i], tempVal[i]));
				}
			}

			return ret;
		}

		//instream for pair
		template<typename BasicTypeA, typename BasicTypeB>
		InStream& operator>> (std::pair<BasicTypeA, BasicTypeB>& a)
		{
			BasicTypeA tempfirst;
			BasicTypeB tempsecond;

			this->operator>> (tempfirst);
			InStream& ret = this->operator>> (tempsecond);
			a = std::make_pair(tempfirst, tempsecond);
			return ret;
		}

		int size() //因为已经砍去了被反序列化的部分
		{
			return total - str.size();
		}

	protected:
		std::string str;
		int total;
	};



	//serialize to a binary file
	template<typename SerializableType>
	void serialize_to_binaryfile(SerializableType& a, std::string filename) {
		OutStream oe;
		oe << a;
		std::string input = oe.str();
		std::ofstream file(filename, std::ios_base::out | std::ios_base::binary);
		if (!file.is_open()) {
			std::cout << "File open error!\n";
			return;
		}
		file.write(input.c_str(), input.size() + 1);
		file.close();
	}

	//deserialize from a binary file
	template<typename SerializableType>
	void desrialize_from_binaryfile(SerializableType& a, std::string filename) {
		std::string output;
		std::ifstream file(filename, std::ios_base::in | std::ios_base::binary);
		if (!file.is_open()) {
			std::cout << "File open error!\n";
			return;
		}
		std::ostringstream os;
		os << file.rdbuf();
		file.close();
		output = os.str();
		InStream ie(output);
		ie >> a;
	}
}//namespace BS
#endif
