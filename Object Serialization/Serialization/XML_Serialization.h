#pragma once
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
#include <string> //std::string
#include <cstdlib>
#include <assert.h> //assert
#include "tinyxml2.h"

namespace XML_Seri {

	using namespace tinyxml2;
	class XMLBase {
	public:
		template<typename T>
		struct VarType
		{
			static void reader(XMLElement * xmlElement, T & value)
			{
				std::cerr << "missing XML reading function for value type : " << typeid(T).name() << std::endl;
				assert(false);
			}
			static void writer(XMLElement * xmlElement, const std::string & name, const T & value)
			{
				std::cerr << "missing XML writing function for value type : " << typeid(T).name() << std::endl;
				assert(false);
			}
		};


		//read&write for container type std::vector<T>
		template<typename T>
		struct VarType<std::vector<T>>
		{
			static void reader(XMLElement *xmlElement, std::vector<T> &value)
			{
				tinyxml2::XMLElement * childElement = xmlElement->FirstChildElement();
				while (childElement)
				{
					T childValue;
					VarType<T>::reader(childElement, childValue);
					value.push_back(childValue);
					childElement = childElement->NextSiblingElement();
				}
			}
			static void writer(XMLElement * xmlElement, const std::string & name, const std::vector<T> & value)
			{
				XMLElement * newElement = xmlElement->GetDocument()->NewElement(name.c_str());
				for (auto &item : value) {
					VarType<T>::writer(newElement, "item", item);
				}
				xmlElement->InsertEndChild(newElement);
			}
		};


		//read&write for container type std::list<T>
		template<typename T>
		struct VarType<std::list<T>>
		{
			static void reader(XMLElement * xmlElement, std::list<T> &value)
			{
				std::vector<T> temp;
				VarType<std::vector<T>>::reader(xmlElement, temp);
				if (temp.size() > 0) {
					std::copy(temp.begin(), temp.end(), std::back_inserter(value));
				}
			}
			static void writer(XMLElement * xmlElement, const std::string & name, const std::list<T> & value)
			{
				std::vector<T> temp;
				std::copy(value.begin(), value.end(), std::back_inserter(temp));
				VarType<std::vector<T>>::writer(xmlElement, name, temp);
			}
		};


		//read&write for container type std::set<T>
		template<typename T>
		struct VarType<std::set<T>>
		{
			static void reader(XMLElement * xmlElement, std::set<T> &value)
			{
				std::vector<T> temp;
				VarType<std::vector<T>>::reader(xmlElement, temp);
				if (temp.size() > 0) {
					for (size_t i = 0; i < temp.size(); ++i)
					{
						value.insert(temp[i]);
					}
				}
			}
			static void writer(XMLElement * xmlElement, const std::string & name, const std::set<T> & value)
			{
				std::vector<T> temp;
				std::copy(value.begin(), value.end(), std::back_inserter(temp));
				VarType<std::vector<T>>::writer(xmlElement, name, temp);
			}
		};


		//read&write for container type std::map<TA,TB>
		template<typename TA, typename TB>
		struct VarType<std::map<TA, TB>>
		{
			static void reader(XMLElement * xmlElement, std::map<TA, TB> &value)
			{
				std::vector<TA> tempKey;
				std::vector<TB> tempVal;
				tinyxml2::XMLElement * KeyElement = xmlElement->FirstChildElement();
				tinyxml2::XMLElement * ValElement = xmlElement->FirstChildElement()->NextSiblingElement();

				if (KeyElement&&ValElement) {
					VarType<std::vector<TA>>::reader(KeyElement, tempKey);
					VarType<std::vector<TB>>::reader(ValElement, tempVal);
				}

				if (tempKey.size() > 0 && tempVal.size() == tempKey.size())
				{
					for (size_t i = 0; i < tempKey.size(); ++i)
					{
						value.insert(std::make_pair(tempKey[i], tempVal[i]));
					}
				}
			}

			static void writer(XMLElement * xmlElement, const std::string & name, const std::map<TA,TB> & value)
			{
				std::vector<TA> tempKey;
				std::vector<TB> tempVal;
				typename std::map<TA, TB>::const_iterator it;
				for (it = value.begin(); it != value.end(); ++it)
				{
					tempKey.push_back(it->first);
					tempVal.push_back(it->second);
				}

				tinyxml2::XMLElement * mapElement = xmlElement->GetDocument()->NewElement(name.c_str());
				VarType<std::vector<TA>>::writer(mapElement, "Key", tempKey);
				VarType<std::vector<TB>>::writer(mapElement, "Val", tempVal);
				xmlElement->InsertEndChild(mapElement);
			}
		};


		//read&write for container type std::pair<TA,TB>
		template<typename TA,typename TB>
		struct VarType<std::pair<TA,TB>>
		{
			static void reader(XMLElement * xmlElement, std::pair<TA, TB> &value)
			{
				TA tempfirst;
				TB tempsecond;

				tinyxml2::XMLElement * firstElement = xmlElement->FirstChildElement();
				tinyxml2::XMLElement * secondElement = xmlElement->FirstChildElement()->NextSiblingElement();

				VarType<TA>::reader(firstElement, tempfirst);
				VarType<TB>::reader(secondElement, tempsecond);

				value = std::make_pair(tempfirst, tempsecond);
			}

			static void writer(XMLElement * xmlElement, const std::string & name, const std::pair<TA, TB> & value)
			{
				TA tempfirst = value.first;
				TB tempsecond = value.second;
				tinyxml2::XMLElement * newElement = xmlElement->GetDocument()->NewElement(name.c_str());
				VarType<TA>::writer(newElement, "first", tempfirst);
				VarType<TB>::writer(newElement, "second", tempsecond);

				xmlElement->InsertEndChild(newElement);
			}
		};

		//read&write for char
		template<>
		struct VarType<char>
		{
			static void reader(tinyxml2::XMLElement * xmlElement, char &ch) {
				memcpy(&ch, xmlElement->GetText(), sizeof(char));
			}

			static void writer(tinyxml2::XMLElement * xmlElement, const std::string & name, const char &ch) {
				tinyxml2::XMLElement * newElement = xmlElement->GetDocument()->NewElement(name.c_str());
				newElement->SetText((const char*)&ch);
				xmlElement->InsertEndChild(newElement);
			}
		};


		//read&write for int
		template<>
		struct VarType < int >
		{
			static void reader(tinyxml2::XMLElement * xmlElement, int &value)
			{
				int tempvalue;
				xmlElement->QueryIntText(&tempvalue);
				value = tempvalue;
			}

			static void writer(tinyxml2::XMLElement * xmlElement, const std::string & name, const int & value)
			{
				tinyxml2::XMLElement * newElement = xmlElement->GetDocument()->NewElement(name.c_str());
				newElement->SetText(value);
				xmlElement->InsertEndChild(newElement);
			}
		};


		//read&write for float
		template<>
		struct VarType < float >
		{
			static void reader(tinyxml2::XMLElement * xmlElement, float &value)
			{
				float tempvalue;
				xmlElement->QueryFloatText(&tempvalue);
				value = tempvalue;
			}

			static void writer(tinyxml2::XMLElement * xmlElement, const std::string & name, const float & value)
			{
				tinyxml2::XMLElement * newElement = xmlElement->GetDocument()->NewElement(name.c_str());
				newElement->SetText(value);
				xmlElement->InsertEndChild(newElement);
			}
		};


		//read&write for double
		template<>
		struct VarType < double >
		{
			static void reader(tinyxml2::XMLElement * xmlElement, double &value)
			{
				double tempvalue;
				xmlElement->QueryDoubleText(&tempvalue);
				value = tempvalue;
			}

			static void writer(tinyxml2::XMLElement * xmlElement, const std::string & name, const double & value)
			{
				tinyxml2::XMLElement * newElement = xmlElement->GetDocument()->NewElement(name.c_str());
				newElement->SetText(value);
				xmlElement->InsertEndChild(newElement);
			}
		};


		//read&write for std::string
		template<>
		struct VarType < std::string >
		{
			static void reader(tinyxml2::XMLElement * xmlElement, std::string &value)
			{
				value = xmlElement->GetText();
			}

			static void writer(tinyxml2::XMLElement * xmlElement, const std::string & name, const std::string & value)
			{
				tinyxml2::XMLElement * newElement = xmlElement->GetDocument()->NewElement(name.c_str());
				newElement->SetText(value.c_str());
				xmlElement->InsertEndChild(newElement);
			}
		};


	};//end XMLBase


	//stream in from xml
	XMLDocument* ReadFromFile(const std::string &file_name) {
		std::ifstream fileStream(file_name);
		std::stringstream stringBuffer;
		stringBuffer << fileStream.rdbuf();
		
		XMLDocument *xmlDoc = new XMLDocument();
		xmlDoc->Parse(stringBuffer.str().c_str());
		return xmlDoc;
	}

	//deserialize from a xml file
	template<typename SerializableType>
	void deserialize_xml(SerializableType& a, const std::string &name, const std::string &file_name) {
		XMLDocument *xmlDoc = ReadFromFile(file_name);
		XMLElement *xmlNode = xmlDoc->RootElement()->FirstChildElement();
		if (xmlNode) {
			XMLBase::VarType<SerializableType>::reader(xmlNode, a);
		}
	}

	//stream out to xml
	void WriteToFile(const XMLDocument *xmlDoc, const std::string &file_name) {
		XMLPrinter xmlPrinter;
		xmlDoc->Print(&xmlPrinter);
		std::string buffer = std::string(xmlPrinter.CStr());
		std::ofstream fileStream(file_name);
		fileStream << buffer;
	}

	//serialize to a xml file
	template<typename SerializableType>
	void serialize_xml(SerializableType& a, const std::string &name, const std::string &file_name) {
		XMLDocument* xmlDoc = new XMLDocument();
		XMLDeclaration * xmlDecl = xmlDoc->NewDeclaration();
		xmlDoc->InsertFirstChild(xmlDecl);

		XMLElement * rootElement = xmlDoc->NewElement("serialization");
		XMLBase::VarType<SerializableType>::writer(rootElement, name, a);
		xmlDoc->InsertEndChild(rootElement);

		WriteToFile(xmlDoc, file_name);
	}

	////////////////////////////////////////////
	//Serialize for custom class object
	//If your class object want to be serialized,
	//Plese derive for this base class
	///////////////////////////////////////////
	class xmlSerialize {
		virtual void deserialize_xml(const std::string &name, const std::string &file_name) = 0;
		virtual void serialize_xml(const std::string &name, const std::string &file_name) = 0;
	};


}
