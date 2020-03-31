/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010-2014 Joel Andersson, Joris Gillis, Moritz Diehl,
 *                            K.U. Leuven. All rights reserved.
 *    Copyright (C) 2011-2014 Greg Horn
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include "tinyxml_interface.hpp"

using namespace std;
namespace casadi {

  extern "C"
  int CASADI_XMLFILE_TINYXML_EXPORT
  casadi_register_xmlfile_tinyxml(XmlFileInternal::Plugin* plugin) {
    plugin->creator = TinyXmlInterface::creator;
    plugin->name = "tinyxml";
    plugin->doc = TinyXmlInterface::meta_doc.c_str();
    plugin->version = 21;
    return 0;
  }

  extern "C"
  void CASADI_XMLFILE_TINYXML_EXPORT casadi_load_xmlfile_tinyxml() {
    XmlFileInternal::registerPlugin(casadi_register_xmlfile_tinyxml);
  }

  TinyXmlInterface::TinyXmlInterface() : XmlFileInternal() {
  }

  TinyXmlInterface::~TinyXmlInterface() {
  }

  TinyXmlInterface* TinyXmlInterface::clone() const {
    return new TinyXmlInterface();
  }

  XmlNode TinyXmlInterface::parse(const std::string& filename) {
    bool flag = doc_.LoadFile(filename.c_str());
    casadi_assert_message(flag, "Cound not open " << filename);
    return addNode(&doc_);
  }

  XmlNode TinyXmlInterface::addNode(TiXmlNode* n) {
    if (!n) throw CasadiException("Error in TinyXmlInterface::addNode: Node is 0");
    XmlNode ret;

    // Save name
    ret.setName(n->Value());

    // Save attributes
    int type = n->Type();
    if (type == TiXmlNode::TINYXML_ELEMENT) {
      if (n->ToElement()!=0) {
        for (TiXmlAttribute* pAttrib=n->ToElement()->FirstAttribute();
             pAttrib;
             pAttrib=pAttrib->Next()) {
          ret.setAttribute(pAttrib->Name(), pAttrib->Value());
        }
      }
    } else if (type == TiXmlNode::TINYXML_DOCUMENT) {
      // do nothing
    } else {
      throw CasadiException("TinyXmlInterface::addNode");
    }

    // Count the number of children
    int num_children = 0;
    for (TiXmlNode* child = n->FirstChild(); child != 0; child= child->NextSibling()) {
      num_children++;
    }
    ret.children_.reserve(num_children);

    // add children
    int ch = 0;
    for (TiXmlNode* child = n->FirstChild(); child != 0; child= child->NextSibling(), ++ch) {
      int childtype = child->Type();

      if (childtype == TiXmlNode::TINYXML_ELEMENT) {
        XmlNode newnode = addNode(child);
        ret.children_.push_back(newnode);
        ret.child_indices_[newnode.getName()] = ch;
      } else if (childtype == TiXmlNode::TINYXML_COMMENT) {
        ret.comment_ = child->Value();
      } else if (childtype == TiXmlNode::TINYXML_TEXT) {
        ret.text_ = child->ToText()->Value();
      } else if (childtype == TiXmlNode::TINYXML_DECLARATION) {
        cout << "Warning: Skipped TiXmlNode::TINYXML_DECLARATION" << endl;
      } else {
        throw CasadiException("Error in TinyXmlInterface::addNode: Unknown node type");
      }
    }

    // Note: Return value optimization
    return ret;
  }


} // namespace casadi
