#pragma once
#include "Serialization.h"
#include "XML_Serialization.h"

//UserDefinedType for binary serialization
class cbox : public BS::Serializable
{
public:
	int a;
	double b;
	std::string str;

public:
	cbox()
	{
		str.clear();
		a = 0;
		b = 0.0;
	}
	cbox(const int a, const double b, const char *str) :a(a), b(b), str(str) {}
	cbox(const cbox& other) :a(other.a), b(other.b), str(other.str) {}
	~cbox() { str.clear(); }

	cbox& operator=(const cbox& other)
	{
		if (this != &other)
		{
			a = other.a;
			b = other.b;
			str = other.str;
		}
		return *this;
	}

	bool operator==(const cbox& other)
	{
		if (this == &other)
		{
			return true;
		}
		else if (this->a == other.a&&this->b == other.b&&this->str == other.str) {
			return true;
		}
		else
			return false;
	}

	virtual std::string serialize()
	{
		BS::OutStream oe;
		oe << a << b << str;
		return oe.str();
	}

	virtual int deserialize(std::string &s)
	{
		BS::InStream x(s);
		x >> a >> b >> str;
		return x.size();
	}

	void display() {
		std::cout << "a:" << a << ", b: " << b << ", str: " << str << std::endl;
	}
};

//UserDefinedType for xml serialization
class xbox: public XML_Seri::xmlSerialize
{
public:
	int a;
	double b;
	std::string str;

public:
	xbox()
	{
		str.clear();
		a = 0;
		b = 0.0;
	}
	xbox(const int a, const double b, const char *str) :a(a), b(b), str(str) {}
	xbox(const xbox& other) :a(other.a), b(other.b), str(other.str) {}
	~xbox() { str.clear(); }

	xbox& operator=(const xbox& other)
	{
		if (this != &other)
		{
			a = other.a;
			b = other.b;
			str = other.str;
		}
		return *this;
	}

	bool operator==(const xbox& other)
	{
		if (this == &other)
		{
			return true;
		}
		else if (this->a == other.a&&this->b == other.b&&this->str == other.str) {
			return true;
		}
		else
			return false;
	}


	void display() {
		std::cout << "a:" << a << ", b: " << b << ", str: " << str << std::endl;
	}

	void deserialize_xml(const std::string &name, const std::string &file_name) {
		tinyxml2::XMLDocument *xmlDoc = XML_Seri::ReadFromFile(file_name);
		tinyxml2::XMLElement *xmlNode = xmlDoc->RootElement()->FirstChildElement();
		XML_Seri::XMLBase::VarType<int>::reader(xmlNode, a);
		xmlNode = xmlNode->NextSiblingElement();
		XML_Seri::XMLBase::VarType<double>::reader(xmlNode, b);
		xmlNode = xmlNode->NextSiblingElement();
		XML_Seri::XMLBase::VarType<std::string>::reader(xmlNode, str);
	}

	void serialize_xml(const std::string &name, const std::string &file_name) {
		tinyxml2::XMLDocument* xmlDoc = new tinyxml2::XMLDocument();
		tinyxml2::XMLDeclaration * xmlDecl = xmlDoc->NewDeclaration();
		xmlDoc->InsertFirstChild(xmlDecl);

		tinyxml2::XMLElement * rootElement = xmlDoc->NewElement(name.c_str());
		XML_Seri::XMLBase::VarType<int>::writer(rootElement, "int", a);
		XML_Seri::XMLBase::VarType<double>::writer(rootElement, "double", b);
		XML_Seri::XMLBase::VarType<std::string>::writer(rootElement, "std_str", str);

		xmlDoc->InsertEndChild(rootElement);
		XML_Seri::WriteToFile(xmlDoc, file_name);
	}

};

//======TEST======================================================================
template<typename TypeA>
void ASSERT_EQ(TypeA f, TypeA f1) {
	if (f == f1) {
		std::cout << "Equal_Test: true\n";
	}
	else {
		std::cout << "Equal_Test: false\n";
	}
}
void ASSERT_TRUE(bool c) {
	if (c) {
		std::cout << "TRUE_Test: success\n";
	}
	else {
		std::cout << "TRUE_Test: fail\n";
	}
}

void TEST_UserDefined() {
	std::cout << "====================================\n";
	std::cout << "===TEST_UserDefined=================\n";
	std::cout << "====================================\n";

	//binary_test
	cbox box(11, 6.6, "Hello World");
	cbox box1;
	BS::serialize_to_binaryfile(box, "test_file\\test_userdefined.data");
	BS::desrialize_from_binaryfile(box1, "test_file\\test_userdefined.data");
	std::cout << "binary_userdefined_test\n";
	ASSERT_EQ(box, box1);

	//xml_test
	xbox pox(11, 6.6, "Hello World");
	xbox pox1;
	pox.serialize_xml("userdefined", "test_file\\test_userdefined.xml");
	pox1.deserialize_xml("userdefined", "test_file\\test_userdefined.xml");
	std::cout << "xml_userdefined_test\n";
	ASSERT_EQ(pox, pox1);

}

void TEST_BasicType() {
	std::cout << "\n====================================\n";
	std::cout << "===TEST_BasicType=================\n";
	std::cout << "====================================\n";

	//char_test
	char x = 'a', x1, x2;
	BS::serialize_to_binaryfile(x, "test_file\\test_char.data");
	BS::desrialize_from_binaryfile(x1, "test_file\\test_char.data");
	std::cout << "binary_char_test\n";
	ASSERT_EQ(x, x1);
	
	XML_Seri::serialize_xml(x, "char", "test_file\\test_char.xml");
	XML_Seri::deserialize_xml(x2, "char", "test_file\\test_char.xml");
	std::cout << "xml_char_test\n";
	ASSERT_EQ(x, x2);

	//int_test
	int a = 2, a1, a2;
	BS::serialize_to_binaryfile(a, "test_file\\test_int.data");
	BS::desrialize_from_binaryfile(a1, "test_file\\test_int.data");
	std::cout << "==================================\n";
	std::cout << "binary_int_test\n";
	ASSERT_EQ(a, a1);

	XML_Seri::serialize_xml(a, "int", "test_file\\test_int.xml");
	XML_Seri::deserialize_xml(a2, "int", "test_file\\test_int.xml");
	std::cout << "xml_int_test\n";
	ASSERT_EQ(a, a2);


	//float_test
	float b = 4, b1, b2;
	BS::serialize_to_binaryfile(b, "test_file\\test_float.data");
	BS::desrialize_from_binaryfile(b1, "test_file\\test_float.data");
	std::cout << "==================================\n";
	std::cout << "binary_float_test\n";
	ASSERT_EQ(b, b1);

	XML_Seri::serialize_xml(b, "float", "test_file\\test_float.xml");
	XML_Seri::deserialize_xml(b2, "float", "test_file\\test_float.xml");
	std::cout << "xml_float_test\n";
	ASSERT_EQ(b, b2);

}

void TEST_String() {
	std::cout << "\n====================================\n";
	std::cout << "===TEST_String=================\n";
	std::cout << "====================================\n";
	
	std::string f = "hello world!", f1, f2;
	//binary_test
	BS::serialize_to_binaryfile(f, "test_file\\test_string.data");
	BS::desrialize_from_binaryfile(f1, "test_file\\test_string.data");
	std::cout << "binary_string_test\n";
	ASSERT_EQ(f, f1);

	//xml_test
	XML_Seri::serialize_xml(f, "std_str", "test_file\\test_string.xml");
	XML_Seri::deserialize_xml(f2, "std_str", "test_file\\test_string.xml");
	std::cout << "xml_string_test\n";
	ASSERT_EQ(f, f2);
}

void TEST_Vector() {
	std::cout << "\n====================================\n";
	std::cout << "===TEST_Vector=================\n";
	std::cout << "====================================\n";

	std::vector<std::string> n, n1, n2;
	n.push_back("oop");
	n.push_back("OOP");
	n.push_back("great");
	n.push_back("fantastic");

	//binary_test
	BS::serialize_to_binaryfile(n, "test_file\\test_vector.data");
	BS::desrialize_from_binaryfile(n1, "test_file\\test_vector.data");
	std::cout << "binary_vector_test\n";
	std::cout << "size_equal?";
	ASSERT_EQ(n.size(), n1.size());
	std::cout << "value_equal?";
	for (size_t i = 0; i < n.size(); ++i)
	{
		ASSERT_TRUE(n[i] == n1[i]);
	}


	//xml_test
	XML_Seri::serialize_xml(n, "std_vector", "test_file\\test_vector.xml");
	XML_Seri::deserialize_xml(n2, "std_vector", "test_file\\test_vector.xml");
	std::cout << "xml_vector_test\n";
	std::cout << "size_equal?";
	ASSERT_EQ(n.size(), n2.size());
	std::cout << "value_equal?";
	for (size_t i = 0; i < n.size(); ++i)
	{
		ASSERT_TRUE(n[i] == n2[i]);
	}
}

void TEST_List() {
	std::cout << "\n====================================\n";
	std::cout << "===TEST_List=================\n";
	std::cout << "====================================\n";

	std::list<std::string> s, s1, s2;
	s.push_back("hello");
	s.push_back("world");
	s.push_back("yeah!");
	s.push_back("haha");

	//binary_test
	std::cout << "binary_List_test\n";
	BS::serialize_to_binaryfile(s, "test_file\\test_list.data");
	BS::desrialize_from_binaryfile(s1, "test_file\\test_list.data");

	std::cout << "size_equal?\n";
	ASSERT_EQ(s.size(), s1.size());

	std::cout << "value_equal?\n";
	std::list<std::string>::iterator it, itnew;
	for (it = s.begin(), itnew = s1.begin(); it != s.end() && itnew != s1.end(); ++it, ++itnew)
	{
		ASSERT_EQ((*it), (*itnew));
		ASSERT_TRUE((*it) == (*itnew));
		//std::cout << (*itnew);
	}

	//xml_test
	std::cout << "XML_List_test\n";
	XML_Seri::serialize_xml(s, "std_list", "test_file\\test_list.xml");
	XML_Seri::deserialize_xml(s2, "std_list", "test_file\\test_list.xml");
	
	std::cout << "size_equal?\n";
	ASSERT_EQ(s.size(), s2.size());

	std::cout << "value_equal?\n";
	for (it = s.begin(), itnew = s2.begin(); it != s.end() && itnew != s2.end(); ++it, ++itnew)
	{
		ASSERT_EQ((*it), (*itnew));
		ASSERT_TRUE((*it) == (*itnew));
		//std::cout << (*itnew);
	}
}

void TEST_Set() {
	std::cout << "\n====================================\n";
	std::cout << "===TEST_Set=================\n";
	std::cout << "====================================\n";

	std::set<std::string> s, s1, s2;
	s.insert("hello");
	s.insert("world");
	s.insert("yeah!");
	s.insert("yeah!");

	//binary_test
	std::cout << "binary_Set_test\n";
	BS::serialize_to_binaryfile(s, "test_file\\test_set.data");
	BS::desrialize_from_binaryfile(s1, "test_file\\test_set.data");
	
	std::cout << "size_equal?\n";
	ASSERT_EQ(s.size(), s1.size());

	std::cout << "value_equal?\n";
	std::set<std::string>::iterator it, itnew;

	for (it = s.begin(), itnew = s1.begin(); it != s.end() && itnew != s1.end(); ++it, ++itnew)
	{
		ASSERT_TRUE((*it) == (*itnew));
	}

	//xml_test
	XML_Seri::serialize_xml(s, "std_set", "test_file\\test_set.xml");
	XML_Seri::deserialize_xml(s2, "std_set", "test_file\\test_set.xml");
	std::cout << "size_equal?\n";
	ASSERT_EQ(s.size(), s2.size());

	std::cout << "value_equal?\n";
	for (it = s.begin(), itnew = s2.begin(); it != s.end() && itnew != s2.end(); ++it, ++itnew)
	{
		ASSERT_TRUE((*it) == (*itnew));
	}

}

void TEST_Map() {
	std::cout << "\n====================================\n";
	std::cout << "===TEST_Map=================\n";
	std::cout << "====================================\n";

	std::map<std::string, int> m, m1, m2;
	m["first"] = 1;
	m["second"] = 2;
	m["third"] = 3;

	//binary_test
	std::cout << "binary_Map_test\n";
	BS::serialize_to_binaryfile(m, "test_file\\test_map.data");
	BS::desrialize_from_binaryfile(m1, "test_file\\test_map.data");

	std::cout << "size_equal?\n";
	ASSERT_EQ(m.size(), m1.size());

	std::cout << "value_equal?\n";
	std::map<std::string, int>::const_iterator it, itnew;
	for (it = m.begin(), itnew = m1.begin(); it != m.end() && itnew != m1.end(); ++it, ++itnew)
	{
		ASSERT_TRUE(it->first == itnew->first);
		ASSERT_TRUE(it->second == itnew->second);
	}

	//xml_test
	std::cout << "xml_Map_test\n";
	XML_Seri::serialize_xml(m, "std_map", "test_file\\test_map.xml");
	XML_Seri::deserialize_xml(m2, "std_map", "test_file\\test_map.xml");
	std::cout << "size_equal?\n";
	ASSERT_EQ(m.size(), m2.size());

	std::cout << "value_equal?\n";
	for (it = m.begin(), itnew = m2.begin(); it != m.end() && itnew != m2.end(); ++it, ++itnew)
	{
		ASSERT_TRUE(it->first == itnew->first);
		ASSERT_TRUE(it->second == itnew->second);
	}
}

void TEST_Pair() {
	std::cout << "\n====================================\n";
	std::cout << "===TEST_Pair=================\n";
	std::cout << "====================================\n";

	std::pair<int, double> p(2, 3.1), p1, p2;

	//binary_test
	BS::serialize_to_binaryfile(p, "test_file\\test_pair.data");
	BS::desrialize_from_binaryfile(p1, "test_file\\test_pair.data");
	ASSERT_TRUE(p.first == p1.first);
	ASSERT_TRUE(p.second == p1.second);

	//xml_test
	XML_Seri::serialize_xml(p, "std_pair", "test_file\\test_pair.xml");
	XML_Seri::deserialize_xml(p2, "std_pair", "test_file\\test_pair.xml");
	ASSERT_TRUE(p.first == p2.first);
	ASSERT_TRUE(p.second == p2.second);

}

