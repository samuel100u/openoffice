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


#ifndef SW_VBA_OPTIONS_HXX
#define SW_VBA_OPTIONS_HXX

#include <ooo/vba/word/XOptions.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <cppuhelper/implbase1.hxx>
#include <vbahelper/vbapropvalue.hxx>
#include <comphelper/processfactory.hxx>

typedef InheritedHelperInterfaceImpl1< ooo::vba::word::XOptions > SwVbaOptions_BASE;

class SwVbaOptions : public SwVbaOptions_BASE, 
                    public PropListener
{
private:
    rtl::OUString msDefaultFilePath;
    css::uno::Reference< css::lang::XMultiServiceFactory > mxFactory;
public:
	SwVbaOptions( css::uno::Reference< css::uno::XComponentContext >& m_xContext ) throw ( css::uno::RuntimeException );
	virtual ~SwVbaOptions();

    // Attributes
    virtual ::sal_Int32 SAL_CALL getDefaultBorderLineStyle() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setDefaultBorderLineStyle( ::sal_Int32 _defaultborderlinestyle ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getDefaultBorderLineWidth() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setDefaultBorderLineWidth( ::sal_Int32 _defaultborderlinewidth ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getDefaultBorderColorIndex() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setDefaultBorderColorIndex( ::sal_Int32 _defaultbordercolorindex ) throw (css::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL getReplaceSelection() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setReplaceSelection( ::sal_Bool _replaceselection ) throw (css::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL getMapPaperSize() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setMapPaperSize( ::sal_Bool _mappapersize ) throw (css::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL getAutoFormatAsYouTypeApplyHeadings() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setAutoFormatAsYouTypeApplyHeadings( ::sal_Bool _autoformatasyoutypeapplyheadings ) throw (css::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL getAutoFormatAsYouTypeApplyBulletedLists() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setAutoFormatAsYouTypeApplyBulletedLists( ::sal_Bool _autoformatasyoutypeapplybulletedlists ) throw (css::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL getAutoFormatAsYouTypeApplyNumberedLists() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setAutoFormatAsYouTypeApplyNumberedLists( ::sal_Bool _autoformatasyoutypeapplynumberedlists ) throw (css::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL getAutoFormatAsYouTypeFormatListItemBeginning() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setAutoFormatAsYouTypeFormatListItemBeginning( ::sal_Bool _autoformatasyoutypeformatlistitembeginning ) throw (css::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL getAutoFormatAsYouTypeDefineStyles() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setAutoFormatAsYouTypeDefineStyles( ::sal_Bool _autoformatasyoutypedefinestyles ) throw (css::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL getAutoFormatApplyHeadings() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setAutoFormatApplyHeadings( ::sal_Bool _autoformatapplyheadings ) throw (css::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL getAutoFormatApplyLists() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setAutoFormatApplyLists( ::sal_Bool _autoformatapplylists ) throw (css::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL getAutoFormatApplyBulletedLists() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setAutoFormatApplyBulletedLists( ::sal_Bool _autoformatapplybulletedlists ) throw (css::uno::RuntimeException);

    // Methods
    virtual css::uno::Any SAL_CALL DefaultFilePath( sal_Int32 _path ) throw ( css::uno::RuntimeException );

    //PropListener
    virtual void setValueEvent( const css::uno::Any& value );
    virtual css::uno::Any getValueEvent();

	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif /* SW_VBA_OPTIONS_HXX */
