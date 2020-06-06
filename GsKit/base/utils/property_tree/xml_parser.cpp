#include "xml_parser.h"

#ifdef BOOST_ENABLED
#include <boost/property_tree/xml_parser.hpp>
#include <boost/version.hpp>
#else
#include "tinyxml2.h"

#include <base/utils/FindFile.h>

using namespace tinyxml2;

#endif // BOOST_ENABLED

namespace GsKit
{

#ifdef BOOST_ENABLED

  #if BOOST_VERSION >= 105600
    typedef boost::property_tree::xml_writer_settings<std::string> xml_writer_settings;
  #else
    typedef boost::property_tree::xml_writer_settings<char> xml_writer_settings;
  #endif


static void addNodeFromBoostPTree(GsKit::ptree &output,
                                  boost::property_tree::ptree &input)
{
    for( auto &innode : input )
    {
        GsKit::ptree newChild;
        addNodeFromBoostPTree(newChild, innode.second);
        output.add_child(innode.first, newChild );
    }

    output.data() = input.data();
}

static void addNodeToBoostPTree(boost::property_tree::ptree &output,
                                GsKit::ptree &input)
{
    for( auto &innode : input )
    {
        boost::property_tree::ptree newChild;
        addNodeToBoostPTree(newChild, innode.second);
        output.add_child(innode.first, newChild );
    }

    output.data() = input.data();
}

static void read_xml_boost(const std::string &filename, GsKit::ptree &pt)
{
    boost::property_tree::ptree tree;

    boost::property_tree::xml_parser::read_xml(filename, tree);

    addNodeFromBoostPTree(pt, tree);
}

static void read_xml_boost(std::ifstream &file, GsKit::ptree &pt)
{
    boost::property_tree::ptree tree;

    boost::property_tree::xml_parser::read_xml(file, tree);

    addNodeFromBoostPTree(pt, tree);

}


static void write_xml_boost(std::ofstream &file, GsKit::ptree &pt)
{
    boost::property_tree::ptree tree;

    addNodeToBoostPTree(tree, pt);

    xml_writer_settings settings('\t', 1);
    boost::property_tree::xml_parser::write_xml(file, tree, settings);
}

static void read_xml_tinyxml2(std::ifstream &file, GsKit::ptree &pt)
{
    /*boost::property_tree::ptree tree;

    boost::property_tree::xml_parser::read_xml(file, tree);

    addNodeFromBoostPTree(pt, tree);
*/
}


static void write_xml_tinyxml2(std::ofstream &file, GsKit::ptree &pt)
{
    /*
    boost::property_tree::ptree tree;

    addNodeToBoostPTree(tree, pt);

    xml_writer_settings settings('\t', 1);
    boost::property_tree::xml_parser::write_xml(file, tree, settings);
    */
}

#endif

#include <cstdio>

static void addNodeFromTinyXML(GsKit::ptree &output,
                               XMLNode *input)
{
    if(input == nullptr)
        return;

    auto curNode = input;

    while(curNode != nullptr)
    {
        if(auto elem = curNode->ToElement())
        {
            auto attr = elem->FirstAttribute();

            GsKit::ptree gsChild;

            if(attr != nullptr)
            {
                GsKit::ptree xmlnodes;

                while( attr != nullptr )
                {
                    std::string attrName = attr->Name();
                    std::string attrValue = attr->Value();

                    xmlnodes.add(attrName, attrValue);
                    attr = attr->Next();
                }

                gsChild.add_child("<xmlattr>", xmlnodes);
            }

            std::string value = elem->Value();

            auto subNode   = elem->FirstChild();

            if(subNode)
            {
                auto subVal = subNode->Value();
                gsChild.data() = subVal;
            }

            auto siblingChild = curNode->FirstChild();
            addNodeFromTinyXML(gsChild, siblingChild);
            output.add_child(value, gsChild);
        }

        curNode = curNode->NextSibling();
    }

}

static void addNodeToTinyXML(GsKit::ptree &input,
                             XMLNode *output,
                             XMLDocument *doc)
{
    for( auto &data : input )
    {
        std::string myFirst = data.first;

        auto curElem = output->ToElement();

        if(myFirst == "<xmlattr>")
        {
            for(auto &attr : data.second)
            {
                const auto name  = attr.first;
                const auto value = attr.second.data();

                curElem->SetAttribute(name.c_str(), value.c_str());
            }
        }
        else
        {
            XMLNode * node = doc->NewElement(myFirst.c_str());

            if(myFirst == "lifes")
            {
                printf("Stop");
            }

            output->InsertEndChild(node);

            addNodeToTinyXML(data.second, node, doc);
        }
    }

    if(!input.data().empty())
    {
        auto elem = output->ToElement();
        elem->SetText(input.data().c_str());
    }
}

static void read_xml_tinyxml2(const std::string &filename, GsKit::ptree &pt)
{
    XMLDocument doc;

    auto fullfn = GetFullFileName(filename);

    doc.LoadFile(fullfn.c_str());

    XMLNode * pRoot = doc.FirstChild();

    addNodeFromTinyXML(pt, pRoot);
}


static void write_xml_tinyxml2(const std::string &filename, GsKit::ptree &pt)
{
    XMLDocument xmlDoc;

    auto decl = xmlDoc.NewDeclaration(nullptr);
    xmlDoc.InsertEndChild(decl);

    for( auto &data : pt )
    {                
        std::string firstStr(data.first);
        XMLElement * newElem = xmlDoc.NewElement(firstStr.c_str());
        xmlDoc.InsertEndChild(newElem);

        addNodeToTinyXML(data.second, newElem, &xmlDoc);
    }    

    auto fullfn = GetWriteFullFileName(filename);
    xmlDoc.SaveFile(fullfn.c_str());
}



void read_xml(const std::string &filename, GsKit::ptree &pt)
{
    #ifdef BOOST_ENABLED
        read_xml_boost(filename, pt);
    #else
        read_xml_tinyxml2(filename, pt);
    #endif
}

#ifdef BOOST_ENABLED
void read_xml(std::ifstream &file, GsKit::ptree &pt)
{  
    read_xml_boost(file, pt);
}
#endif


#ifdef BOOST_ENABLED
void write_xml(std::ofstream &file, GsKit::ptree &pt)
{    
    write_xml_boost(file, pt);
}
#endif

void write_xml(const std::string &filename, GsKit::ptree &pt)
{
    write_xml_tinyxml2(filename, pt);
}


};
