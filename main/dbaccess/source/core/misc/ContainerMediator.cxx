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
#include "precompiled_dbaccess.hxx"
#ifndef DBA_CONTAINERMEDIATOR_HXX
#include "ContainerMediator.hxx"
#endif
#ifndef DBACCESS_SHARED_DBASTRINGS_HRC
#include "dbastrings.hrc"
#endif
#ifndef DBA_PROPERTYSETFORWARD_HXX
#include "PropertyForward.hxx"
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XCOLUMNSSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XTABLESSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XTablesSupplier.hpp>
#endif
#include <com/sun/star/sdbcx/XRename.hpp>
#ifndef _CONNECTIVITY_DBTOOLS_HXX_
#include <connectivity/dbtools.hxx>
#endif
#ifndef _COMPHELPER_PROPERTY_HXX_
#include <comphelper/property.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef TOOLS_DIAGNOSE_EX_H
#include <tools/diagnose_ex.h>
#endif


//........................................................................
namespace dbaccess
{
//........................................................................
	using namespace ::com::sun::star::uno;
	using namespace ::com::sun::star::lang;
	using namespace ::com::sun::star::sdbc;
	using namespace ::com::sun::star::sdbcx;
	using namespace ::com::sun::star::beans;
	using namespace ::com::sun::star::container;

DBG_NAME(OContainerMediator)
OContainerMediator::OContainerMediator( const Reference< XContainer >& _xContainer, const Reference< XNameAccess >& _xSettings,
    const Reference< XConnection >& _rxConnection, ContainerType _eType )
	: m_xSettings( _xSettings )
    , m_xContainer( _xContainer )
    , m_aConnection( _rxConnection )
	, m_eType( _eType )
{
	DBG_CTOR(OContainerMediator,NULL);	
	
    if ( _xSettings.is() && _xContainer.is() )
    {
        osl_incrementInterlockedCount(&m_refCount);
	    try
	    {
		    m_xContainer->addContainerListener(this);
		    Reference< XContainer >	xContainer(_xSettings, UNO_QUERY);
		    if ( xContainer.is() )
			    xContainer->addContainerListener(this);
	    }
	    catch(Exception&)
	    {
		    OSL_ENSURE(sal_False, "OContainerMediator::OContainerMediator: caught an exception!");
	    }
        osl_decrementInterlockedCount( &m_refCount );
    }
    else
    {
        m_xSettings.clear();
        m_xContainer.clear();
    }
}
// -----------------------------------------------------------------------------
OContainerMediator::~OContainerMediator()
{
	DBG_DTOR(OContainerMediator,NULL);	
    acquire();
    impl_cleanup_nothrow();
}

// -----------------------------------------------------------------------------
void OContainerMediator::impl_cleanup_nothrow()
{
    try
    {
		Reference< XContainer >	xContainer( m_xSettings, UNO_QUERY );
		if ( xContainer.is() )
			xContainer->removeContainerListener( this );
		m_xSettings.clear();

        xContainer = m_xContainer;
		if ( xContainer.is() )
			xContainer->removeContainerListener( this );
		m_xContainer.clear();

        m_aForwardList.clear();
    }
    catch( const Exception& )
    {
    	DBG_UNHANDLED_EXCEPTION();
    }
}

// -----------------------------------------------------------------------------
void SAL_CALL OContainerMediator::elementInserted( const ContainerEvent& _rEvent ) throw(RuntimeException)
{		
	::osl::MutexGuard aGuard(m_aMutex);
	if ( _rEvent.Source == m_xSettings && m_xSettings.is() )
	{
		::rtl::OUString sElementName;
		_rEvent.Accessor >>= sElementName;
		PropertyForwardList::iterator aFind = m_aForwardList.find(sElementName);
		if ( aFind != m_aForwardList.end() )
		{
			Reference< XPropertySet> xDest(_rEvent.Element,UNO_QUERY);
			aFind->second->setDefinition( xDest );
		}
	}
}
// -----------------------------------------------------------------------------
void SAL_CALL OContainerMediator::elementRemoved( const ContainerEvent& _rEvent ) throw(RuntimeException)
{
	::osl::MutexGuard aGuard(m_aMutex);
    Reference< XContainer > xContainer = m_xContainer;
    if ( _rEvent.Source == xContainer && xContainer.is() )
	{
		::rtl::OUString sElementName;
		_rEvent.Accessor >>= sElementName;
		m_aForwardList.erase(sElementName);
        try
        {
	        Reference<XNameContainer> xNameContainer( m_xSettings, UNO_QUERY );
	        if ( xNameContainer.is() && m_xSettings->hasByName( sElementName ) )
		        xNameContainer->removeByName( sElementName );
        }
        catch( const Exception& )
        {
    	    DBG_UNHANDLED_EXCEPTION();
        }
	}
}
// -----------------------------------------------------------------------------
void SAL_CALL OContainerMediator::elementReplaced( const ContainerEvent& _rEvent ) throw(RuntimeException)
{
    Reference< XContainer > xContainer = m_xContainer;
    if ( _rEvent.Source == xContainer && xContainer.is() )
	{
        ::rtl::OUString sElementName;
		_rEvent.ReplacedElement >>= sElementName;
        
        PropertyForwardList::iterator aFind = m_aForwardList.find(sElementName);
		if ( aFind != m_aForwardList.end() )
        {
            ::rtl::OUString sNewName;
            _rEvent.Accessor >>= sNewName;
            try
            {
		        Reference<XNameContainer> xNameContainer( m_xSettings, UNO_QUERY_THROW );
		        if ( xNameContainer.is() && m_xSettings->hasByName( sElementName ) )
                {
                    Reference<XRename> xSource(m_xSettings->getByName(sElementName),UNO_QUERY_THROW);
                    xSource->rename(sNewName);
                }
            }
            catch( const Exception& )
            {
        	    DBG_UNHANDLED_EXCEPTION();
            }

            aFind->second->setName(sNewName);
        }
    }
}

// -----------------------------------------------------------------------------
void SAL_CALL OContainerMediator::disposing( const EventObject& /*Source*/ ) throw(RuntimeException)
{
	::osl::MutexGuard aGuard(m_aMutex);
	
    impl_cleanup_nothrow();
}

// -----------------------------------------------------------------------------
void OContainerMediator::impl_initSettings_nothrow( const ::rtl::OUString& _rName, const Reference< XPropertySet >& _rxDestination )
{
    try
    {
		if ( m_xSettings.is() && m_xSettings->hasByName( _rName ) )
        {
            Reference< XPropertySet > xSettings( m_xSettings->getByName( _rName ), UNO_QUERY_THROW );
            ::comphelper::copyProperties( xSettings, _rxDestination );
        }
    }
    catch( const Exception& )
    {
    	DBG_UNHANDLED_EXCEPTION();
    }
}

// -----------------------------------------------------------------------------
void OContainerMediator::notifyElementCreated( const ::rtl::OUString& _sName, const Reference< XPropertySet >& _xDest )
{
    if ( !m_xSettings.is() )
        return;

	PropertyForwardList::iterator aFind = m_aForwardList.find( _sName );
	if  (   aFind != m_aForwardList.end()
        &&  aFind->second->getDefinition().is()
        )
    {
        OSL_ENSURE( false, "OContainerMediator::notifyElementCreated: is this really a valid case?" );
        return;
    }

    ::std::vector< ::rtl::OUString > aPropertyList;
    try
    {
        // initially copy from the settings object (if existent) to the newly created object
        impl_initSettings_nothrow( _sName, _xDest );

        // collect the to-be-monitored properties
        Reference< XPropertySetInfo > xPSI( _xDest->getPropertySetInfo(), UNO_QUERY_THROW );
        Sequence< Property > aProperties( xPSI->getProperties() );
        const Property* property = aProperties.getConstArray();
        const Property* propertyEnd = aProperties.getConstArray() + aProperties.getLength();
        for ( ; property != propertyEnd; ++property )
        {
            if ( ( property->Attributes & PropertyAttribute::READONLY ) != 0 )
                continue;
            if ( ( property->Attributes & PropertyAttribute::BOUND ) == 0 )
                continue;

            aPropertyList.push_back( property->Name );
        }
    }
    catch( const Exception& )
    {
    	DBG_UNHANDLED_EXCEPTION();
    }

    ::rtl::Reference< OPropertyForward > pForward( new OPropertyForward( _xDest, m_xSettings, _sName, aPropertyList ) );
    m_aForwardList[ _sName ] = pForward;
}
// -----------------------------------------------------------------------------
//........................................................................
}	// namespace dbaccess
//........................................................................
