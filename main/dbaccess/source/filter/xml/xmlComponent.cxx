/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_dbaxml.hxx"
#ifndef DBA_XMLCOMPONENT_HXX
#include "xmlComponent.hxx"
#endif
#ifndef DBA_XMLFILTER_HXX
#include "xmlfilter.hxx"
#endif
#ifndef _XMLOFF_XMLTOKEN_HXX
#include <xmloff/xmltoken.hxx>
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include <xmloff/xmlnmspe.hxx>
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include <xmloff/nmspmap.hxx>
#endif
#ifndef DBA_XMLENUMS_HXX
#include "xmlEnums.hxx"
#endif
#ifndef DBACCESS_SHARED_XMLSTRINGS_HRC
#include "xmlstrings.hrc"
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef TOOLS_DIAGNOSE_EX_H
#include <tools/diagnose_ex.h>
#endif

namespace dbaxml
{
	using namespace ::com::sun::star::uno;
	using namespace ::com::sun::star::beans;
	using namespace ::com::sun::star::container;
	using namespace ::com::sun::star::xml::sax;
DBG_NAME(OXMLComponent)

OXMLComponent::OXMLComponent( ODBFilter& rImport
				,sal_uInt16 nPrfx
				,const ::rtl::OUString& _sLocalName
				,const Reference< XAttributeList > & _xAttrList
				,const Reference< XNameAccess >& _xParentContainer
				,const ::rtl::OUString& _sComponentServiceName
				) :
	SvXMLImportContext( rImport, nPrfx, _sLocalName )
	,m_bAsTemplate(sal_False)
{
    DBG_CTOR(OXMLComponent,NULL);

	OSL_ENSURE(_xAttrList.is(),"Attribute list is NULL!");
	const SvXMLNamespaceMap& rMap = rImport.GetNamespaceMap();
	const SvXMLTokenMap& rTokenMap = rImport.GetComponentElemTokenMap();

	sal_Int16 nLength = (_xAttrList.is()) ? _xAttrList->getLength() : 0;
	static const ::rtl::OUString s_sTRUE = ::xmloff::token::GetXMLToken(XML_TRUE);
	for(sal_Int16 i = 0; i < nLength; ++i)
	{
		::rtl::OUString sLocalName;
		rtl::OUString sAttrName = _xAttrList->getNameByIndex( i );
		sal_uInt16 nPrefix = rMap.GetKeyByAttrName( sAttrName,&sLocalName );
		rtl::OUString sValue = _xAttrList->getValueByIndex( i );

		switch( rTokenMap.Get( nPrefix, sLocalName ) )
		{
			case XML_TOK_HREF:
				m_sHREF = sValue;
				break;
			case XML_TOK_COMPONENT_NAME:
				m_sName = sValue;
                // sanitize the name. Previously, we allowed to create forms/reports/queries which contain
                // a / in their name, which nowadays is forbidden. To not lose such objects if they're contained
                // in older files, we replace the slash with something less offending.
                m_sName = m_sName.replace( '/', '_' );
				break;
			case XML_TOK_AS_TEMPLATE:
				m_bAsTemplate = (sValue == s_sTRUE ? sal_True : sal_False);
				break;
		}
	}
	if ( m_sHREF.getLength() && m_sName.getLength() && _xParentContainer.is() )
	{
		Sequence< Any > aArguments(3);
		PropertyValue aValue;
		// set as folder
		aValue.Name = PROPERTY_NAME;
		aValue.Value <<= m_sName;
		aArguments[0] <<= aValue;

		aValue.Name = PROPERTY_PERSISTENT_NAME;
		sal_Int32 nIndex = m_sHREF.lastIndexOf('/')+1;
		aValue.Value <<= m_sHREF.getToken(0,'/',nIndex);
		aArguments[1] <<= aValue;

		aValue.Name = PROPERTY_AS_TEMPLATE;
		aValue.Value <<= m_bAsTemplate;
		aArguments[2] <<= aValue;

		try
		{
			Reference< XMultiServiceFactory > xORB( _xParentContainer, UNO_QUERY_THROW );
			Reference< XInterface > xComponent( xORB->createInstanceWithArguments( _sComponentServiceName, aArguments ) );
			Reference< XNameContainer > xNameContainer( _xParentContainer, UNO_QUERY_THROW );
			xNameContainer->insertByName( m_sName, makeAny( xComponent ) );
		}
		catch(Exception&)
		{
            DBG_UNHANDLED_EXCEPTION();
		}
	}
}
// -----------------------------------------------------------------------------

OXMLComponent::~OXMLComponent()
{

    DBG_DTOR(OXMLComponent,NULL);
}
// -----------------------------------------------------------------------------
//----------------------------------------------------------------------------
} // namespace dbaxml
// -----------------------------------------------------------------------------
