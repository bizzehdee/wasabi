#ifndef _WASABI_DEFNODERESULTS_H
#define _WASABI_DEFNODERESULTS_H

class DOMNodeSortByName;
class DOMNode;
class DOMDocument;
class DOMAttr;
class DOMElement;
class DOMCharacterData;
class DOMText;
class DOMCDATASection;
class DOMComment;
class DOMDocumentFragment;
class DOMEntityReference;
class DOMProcessingInstruction;
class DOMDocumentType;
class DOMImplementation;
class DOMNodeList;
class DOMNamedNodeMap;

enum {
 ATTRIBUTE_NODE,              //The node is an Attr.
 CDATA_SECTION_NODE,          //The node is a CDATASection.
 COMMENT_NODE,                //The node is a Comment.
 DOCUMENT_FRAGMENT_NODE,      //The node is a DocumentFragment.
 DOCUMENT_NODE,               //The node is a Document.
 DOCUMENT_TYPE_NODE,          //The node is a DocumentType.
 ELEMENT_NODE,                //The node is an Element.
 ENTITY_NODE,                 //The node is an Entity.
 ENTITY_REFERENCE_NODE,       //The node is an EntityReference.
 NOTATION_NODE,               //The node is a Notation.
 PROCESSING_INSTRUCTION_NODE, //The node is a ProcessingInstruction.
 TEXT_NODE,                   //The node is a Text node.
};


#endif