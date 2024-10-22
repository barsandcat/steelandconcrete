// This file has been generated by Py++.

#include "boost/python.hpp"
#include "python_CEGUI.h"
#include "PropertyDefinitionBase.pypp.hpp"

namespace bp = boost::python;

struct PropertyDefinitionBase_wrapper : CEGUI::PropertyDefinitionBase, bp::wrapper< CEGUI::PropertyDefinitionBase > {

    PropertyDefinitionBase_wrapper(::CEGUI::String const & name, ::CEGUI::String const & help, ::CEGUI::String const & initialValue, bool redrawOnWrite, bool layoutOnWrite )
    : CEGUI::PropertyDefinitionBase( boost::ref(name), boost::ref(help), boost::ref(initialValue), redrawOnWrite, layoutOnWrite )
      , bp::wrapper< CEGUI::PropertyDefinitionBase >(){
        // constructor
    
    }

    virtual void set( ::CEGUI::PropertyReceiver * receiver, ::CEGUI::String const & value ) {
        if( bp::override func_set = this->get_override( "set" ) )
            func_set( boost::python::ptr(receiver), boost::ref(value) );
        else{
            this->CEGUI::PropertyDefinitionBase::set( boost::python::ptr(receiver), boost::ref(value) );
        }
    }
    
    void default_set( ::CEGUI::PropertyReceiver * receiver, ::CEGUI::String const & value ) {
        CEGUI::PropertyDefinitionBase::set( boost::python::ptr(receiver), boost::ref(value) );
    }

    virtual void writeXMLElementType( ::CEGUI::XMLSerializer & xml_stream ) const {
        bp::override func_writeXMLElementType = this->get_override( "writeXMLElementType" );
        func_writeXMLElementType( boost::ref(xml_stream) );
    }

    virtual void writeXMLToStream( ::CEGUI::XMLSerializer & xml_stream ) const  {
        if( bp::override func_writeXMLToStream = this->get_override( "writeXMLToStream" ) )
            func_writeXMLToStream( boost::ref(xml_stream) );
        else{
            this->CEGUI::PropertyDefinitionBase::writeXMLToStream( boost::ref(xml_stream) );
        }
    }
    
    void default_writeXMLToStream( ::CEGUI::XMLSerializer & xml_stream ) const  {
        CEGUI::PropertyDefinitionBase::writeXMLToStream( boost::ref(xml_stream) );
    }

    virtual ::CEGUI::String get( ::CEGUI::PropertyReceiver const * receiver ) const {
        bp::override func_get = this->get_override( "get" );
        return func_get( boost::python::ptr(receiver) );
    }

    virtual ::CEGUI::String getDefault( ::CEGUI::PropertyReceiver const * receiver ) const  {
        if( bp::override func_getDefault = this->get_override( "getDefault" ) )
            return func_getDefault( boost::python::ptr(receiver) );
        else{
            return this->CEGUI::Property::getDefault( boost::python::ptr(receiver) );
        }
    }
    
    ::CEGUI::String default_getDefault( ::CEGUI::PropertyReceiver const * receiver ) const  {
        return CEGUI::Property::getDefault( boost::python::ptr(receiver) );
    }

    virtual bool isDefault( ::CEGUI::PropertyReceiver const * receiver ) const  {
        if( bp::override func_isDefault = this->get_override( "isDefault" ) )
            return func_isDefault( boost::python::ptr(receiver) );
        else{
            return this->CEGUI::Property::isDefault( boost::python::ptr(receiver) );
        }
    }
    
    bool default_isDefault( ::CEGUI::PropertyReceiver const * receiver ) const  {
        return CEGUI::Property::isDefault( boost::python::ptr(receiver) );
    }

    virtual void writeXMLToStream( ::CEGUI::PropertyReceiver const * receiver, ::CEGUI::XMLSerializer & xml_stream ) const  {
        if( bp::override func_writeXMLToStream = this->get_override( "writeXMLToStream" ) )
            func_writeXMLToStream( boost::python::ptr(receiver), boost::ref(xml_stream) );
        else{
            this->CEGUI::Property::writeXMLToStream( boost::python::ptr(receiver), boost::ref(xml_stream) );
        }
    }
    
    void default_writeXMLToStream( ::CEGUI::PropertyReceiver const * receiver, ::CEGUI::XMLSerializer & xml_stream ) const  {
        CEGUI::Property::writeXMLToStream( boost::python::ptr(receiver), boost::ref(xml_stream) );
    }

};

void register_PropertyDefinitionBase_class(){

    { //::CEGUI::PropertyDefinitionBase
        typedef bp::class_< PropertyDefinitionBase_wrapper, bp::bases< CEGUI::Property >, boost::noncopyable > PropertyDefinitionBase_exposer_t;
        PropertyDefinitionBase_exposer_t PropertyDefinitionBase_exposer = PropertyDefinitionBase_exposer_t( "PropertyDefinitionBase", "*!\n\
        \n\
            common base class used for types representing a new property to be\n\
            available on all widgets that use the WidgetLook that the property\n\
            definition is a part of.\n\
        *\n", bp::init< CEGUI::String const &, CEGUI::String const &, CEGUI::String const &, bool, bool >(( bp::arg("name"), bp::arg("help"), bp::arg("initialValue"), bp::arg("redrawOnWrite"), bp::arg("layoutOnWrite") )) );
        bp::scope PropertyDefinitionBase_scope( PropertyDefinitionBase_exposer );
        { //::CEGUI::PropertyDefinitionBase::set
        
            typedef void ( ::CEGUI::PropertyDefinitionBase::*set_function_type )( ::CEGUI::PropertyReceiver *,::CEGUI::String const & ) ;
            typedef void ( PropertyDefinitionBase_wrapper::*default_set_function_type )( ::CEGUI::PropertyReceiver *,::CEGUI::String const & ) ;
            
            PropertyDefinitionBase_exposer.def( 
                "set"
                , set_function_type(&::CEGUI::PropertyDefinitionBase::set)
                , default_set_function_type(&PropertyDefinitionBase_wrapper::default_set)
                , ( bp::arg("receiver"), bp::arg("value") ) );
        
        }
        { //::CEGUI::PropertyDefinitionBase::writeXMLElementType
        
            typedef void ( PropertyDefinitionBase_wrapper::*writeXMLElementType_function_type )( ::CEGUI::XMLSerializer & ) const;
            
            PropertyDefinitionBase_exposer.def( 
                "writeXMLElementType"
                , writeXMLElementType_function_type( &PropertyDefinitionBase_wrapper::writeXMLElementType )
                , ( bp::arg("xml_stream") )
                , "*!\n\
                    \n\
                        Write out the text of the XML element type.  Note that you should\n\
                        not write the opening '<' character, nor any other information such\n\
                        as attributes in this function.  The writeExtraAttributes function\n\
                        can be used for writing attributes.\n\
            \n\
                    @param xml_stream\n\
                        Stream where xml data should be output.\n\
                    *\n" );
        
        }
        { //::CEGUI::PropertyDefinitionBase::writeXMLToStream
        
            typedef void ( ::CEGUI::PropertyDefinitionBase::*writeXMLToStream_function_type )( ::CEGUI::XMLSerializer & ) const;
            typedef void ( PropertyDefinitionBase_wrapper::*default_writeXMLToStream_function_type )( ::CEGUI::XMLSerializer & ) const;
            
            PropertyDefinitionBase_exposer.def( 
                "writeXMLToStream"
                , writeXMLToStream_function_type(&::CEGUI::PropertyDefinitionBase::writeXMLToStream)
                , default_writeXMLToStream_function_type(&PropertyDefinitionBase_wrapper::default_writeXMLToStream)
                , ( bp::arg("xml_stream") ) );
        
        }
        { //::CEGUI::Property::get
        
            typedef ::CEGUI::String ( ::CEGUI::Property::*get_function_type )( ::CEGUI::PropertyReceiver const * ) const;
            
            PropertyDefinitionBase_exposer.def( 
                "get"
                , bp::pure_virtual( get_function_type(&::CEGUI::Property::get) )
                , ( bp::arg("receiver") )
                , "*!\n\
               \n\
                  Return the current value of the Property as a String\n\
            \n\
               @param receiver\n\
                  Pointer to the target object.\n\
            \n\
               @return\n\
                  String object containing a textual representation of the current value of the Property\n\
               *\n" );
        
        }
        { //::CEGUI::Property::getDefault
        
            typedef ::CEGUI::String ( ::CEGUI::Property::*getDefault_function_type )( ::CEGUI::PropertyReceiver const * ) const;
            typedef ::CEGUI::String ( PropertyDefinitionBase_wrapper::*default_getDefault_function_type )( ::CEGUI::PropertyReceiver const * ) const;
            
            PropertyDefinitionBase_exposer.def( 
                "getDefault"
                , getDefault_function_type(&::CEGUI::Property::getDefault)
                , default_getDefault_function_type(&PropertyDefinitionBase_wrapper::default_getDefault)
                , ( bp::arg("receiver") ) );
        
        }
        { //::CEGUI::Property::isDefault
        
            typedef bool ( ::CEGUI::Property::*isDefault_function_type )( ::CEGUI::PropertyReceiver const * ) const;
            typedef bool ( PropertyDefinitionBase_wrapper::*default_isDefault_function_type )( ::CEGUI::PropertyReceiver const * ) const;
            
            PropertyDefinitionBase_exposer.def( 
                "isDefault"
                , isDefault_function_type(&::CEGUI::Property::isDefault)
                , default_isDefault_function_type(&PropertyDefinitionBase_wrapper::default_isDefault)
                , ( bp::arg("receiver") ) );
        
        }
        { //::CEGUI::Property::writeXMLToStream
        
            typedef void ( ::CEGUI::Property::*writeXMLToStream_function_type )( ::CEGUI::PropertyReceiver const *,::CEGUI::XMLSerializer & ) const;
            typedef void ( PropertyDefinitionBase_wrapper::*default_writeXMLToStream_function_type )( ::CEGUI::PropertyReceiver const *,::CEGUI::XMLSerializer & ) const;
            
            PropertyDefinitionBase_exposer.def( 
                "writeXMLToStream"
                , writeXMLToStream_function_type(&::CEGUI::Property::writeXMLToStream)
                , default_writeXMLToStream_function_type(&PropertyDefinitionBase_wrapper::default_writeXMLToStream)
                , ( bp::arg("receiver"), bp::arg("xml_stream") ) );
        
        }
    }

}
