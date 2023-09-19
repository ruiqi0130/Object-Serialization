### Object Serialization

---


*Inspiration came from*

*http://blog.csdn.net/Kiritow/article/details/53129096*

*https://github.com/byebyebryan/lazyxml/tree/master/LazyXML*

---

#### Binary Serialization

* ##### Program description

  This program implements a module to support **binary** serialization/deserialization. The serialized data is stored in the test_file folder as file.data.

  

* ##### Support Types

  * Arithmetic types：`char, int, float, double, string`

  * STL containers：`std::vector, std::list, std::set, std::map, std::pair`

  * User-defined types

    ```c++
    class cbox : public BS::Serializable{
    public:
    	int a;
    	double b;
    	std::string str;    
    }
    ```

  

* ##### Description of function blocks

  * ###### Universal methods of binary serialization and deserialization

    ```c++
    template<typename SerializableType>
    std::string serialize(SerializableType& a)
    {
        return a.serialize();
    }
    
    template<typename SerializableType>
    int deserialize(std::string &str, SerializableType& a) 
    {
    	return a.deserialize(str);
    }
    ```

    

  * ###### Specialization of basic data types (eg：specialization of a string type)

    ```c++
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
    ```

    When serializing a string, the length of the string is serialized along with the contents of the string, thus effectively solving the problem of connecting a string to a string. 

    

  * ###### Template classes of serialization and deserialization

    ```c++
    class Serializable{
    public:
        virtual std::string serialize() = 0; 
        virtual int deserialize(std::string&) = 0;
    };
    ```

    For user-defined classes, you need to inherit to the template class and implement the serialization and deserialization methods yourself. 

    

  * ###### Output engine (Partial display)

    ```c++
    class OutStream
    {
    public:
        OutStream() : os(std::ios::binary)
        {}
        template<typename SerializableType>
        OutStream& operator<< (SerializableType& a)
        {
            std::string x = BS::serialize(a);
            os.write(x.data(), x.size());
            return *this;
        }
        std::string str()
        {
            return os.str();
        }
    public:
        std::ostringstream os;
    };
    ```

    By defining a class for an output engine and overloading the output stream, the type to be serialized is fed into the output engine to obtain the serialized string.

    

    Example for use：

    ```c++
    OutStream oe;
    int a = 1;
    oe << a;
    oe.str(); //Get a serialized string
    ```

    

  * ###### Input engine (Partial display)

    ```c++
    class InStream
    {
    public:
        InStream(std::string &s) : str(s), total(s.size())
        {}
    
        template<typename SerializableType>
        InStream& operator>> (SerializableType& a)
        {
            int ret = BS::deserialize(str, a);
            str = str.substr(ret); 
            return *this;
        }
    
        int size()
        {
        	return total - str.size();
        }
    
    protected:
        std::string str;
        int total;
    };
    ```

    This type accepts serialized strings and deserializes them into the specified type by defining a class for the input engine and overloading the input stream.

    

    Example for use：

    ```c++
    InStream ie(str); //str: a string stored serialized data
    int a;
    ie >> a;
    ```

  

* #####  Test samples (partial presentation)

  ```c++
  void TEST_BasicType()
  {
  	std::cout << "\n====================================\n";
  	std::cout << "===TEST_BasicType=================\n";
  	std::cout << "====================================\n";
  
  	//int_test
  	int a = 2, a1;
  	BS::serialize_to_binaryfile(a, "test_file\\test_int.data");
  	BS::desrialize_from_binaryfile(a1, "test_file\\test_int.data");
  	std::cout << "binary_int_test\n";
  	ASSERT_EQ(a, a1);
  }
  ```

  

* ##### Reference

  http://blog.csdn.net/Kiritow/article/details/53129096

---



#### Xml Serialization

* ##### Program description

  This program implement a wrapper module of [tinyxml2](https://github.com/leethomason/tinyxml2) to support **XML** serialization. The serialized data is stored in the test_file folder as file.xml. 

  

* ##### Support Types

  * Arithmetic types: `char, int, float, double, string`

  * STL containers: `std::vector, std::list, std::set, std::map, std::pair`

  * User-defined types

    ```c++
    class xbox: public XML_Seri::xmlSerialize
    {
    public:
    	int a;
    	double b;
    	std::string str;
    };
    ```

  

* ##### Description of function blocks

  * ###### Universal methods of xml serialization and deserialization

    ```c++
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
    ```

    Defines a class template, including two functions `static void reader(XMLElement * xmlElement, T & value)` and`static void writer(XMLElement * xmlElement, const std::string & name, const T & value)`, which implemente the deserialization and serialization of the specified class `T` respectively. After that, the class template is fully specialized and partially specialized according to different data types, so as to realize the deserialization and serialization functions of different data types.

    

    **Example of full specialization**

    ```c++
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
    ```

    Take advantage of the built-in functions of the TinyXML2 module, `GetText()` and `SetText()` to implement mutual transformation from string to `XMLElement`.

    

    **Example of partial specialization**

    ```c++
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
    ```

    Serialization: Converts each element in the container to an `XMLElement` type, and inserts the converted `XMLElement` type node into the final linked list.

    Deserialization: Use a pointer of type `XMLElement` to traverse the list, converting each node to the specified data type, and use `push_back()` to store it in the container.

    

  * ###### Read and write functions of XML, Wrapper functions of serialization/deserialization (Omit the implementation)

    ```c++
    //stream in from xml
    XMLDocument* ReadFromFile(const std::string &file_name) {}
    
    //deserialize from a xml file
    template<typename SerializableType>
    void deserialize_xml(SerializableType& a, const std::string &name, const std::string &file_name) {}
    
    //stream out to xml
    void WriteToFile(const XMLDocument *xmlDoc, const std::string &file_name) {}
    
    //serialize to a xml file
    template<typename SerializableType>
    void serialize_xml(SerializableType& a, const std::string &name, const std::string &file_name) {}
    ```

    Serialization: convert the target type to an `XMLDocument` type: `XMLDocument *xmlDoc`, and use the function: `xmlDoc->Print(&xmlPrinter)`to write the `XMLDocument` type as a string to the file **file.xml**.

    Deserialization: use `xmlDoc->Parse(str)` to read from **file.xml** and store into the objects of type `XMLDocument`. Use the previous full specialization or partial specialization function: `reader(XMLElement*, T)` to convert an `XMLDocument` type to an object of the specified type.

    

  * ###### User-defined types

    ```c++
    class xmlSerialize {
        virtual void deserialize_xml(const std::string &name, const std::string &file_name) = 0;
        virtual void serialize_xml(const std::string &name, const std::string &file_name) = 0;
    };
    ```

    For user-defined classes, you need to inherit to the template class and implement the serialization `void serialize_xml()` and deserialization `void deserialize_xml()` methods yourself. 

  

* ##### Test samples (partial presentation)

  ```c++
  void TEST_Pair() {
  	std::cout << "\n====================================\n";
  	std::cout << "===TEST_Pair=================\n";
  	std::cout << "====================================\n";
  
  	std::pair<int, double> p(2, 3.1), p2;
  
  	//xml_test
  	XML_Seri::serialize_xml(p, "std_pair", "test_file\\test_pair.xml");
  	XML_Seri::deserialize_xml(p2, "std_pair", "test_file\\test_pair.xml");
  	ASSERT_TRUE(p.first == p2.first);
  	ASSERT_TRUE(p.second == p2.second);
  }
  ```

  

* ##### Reference

  https://github.com/byebyebryan/lazyxml/tree/master/LazyXML
