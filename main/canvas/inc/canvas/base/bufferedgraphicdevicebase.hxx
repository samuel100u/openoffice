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



#ifndef INCLUDED_CANVAS_BUFFEREDGRAPHICDEVICEBASE_HXX
#define INCLUDED_CANVAS_BUFFEREDGRAPHICDEVICEBASE_HXX

#ifndef _COM_SUN_STAR_AWT_XWINDOW2_HPP_
#include <com/sun/star/awt/XWindow2.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XTOPWINDOW_HPP_
#include <com/sun/star/awt/XTopWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XWINDOWLISTENER_HPP_
#include <com/sun/star/awt/XWindowListener.hpp>
#endif

#ifndef INCLUDED_CANVAS_CANVASTOOLS_HXX
#include <canvas/canvastools.hxx>
#endif
#ifndef INCLUDED_CANVAS_GRAPHICDEVICEBASE_HXX
#include <canvas/base/graphicdevicebase.hxx>
#endif


/* Definition of BufferedGraphicDeviceBase class */

namespace canvas
{
    /** Helper template base class for XGraphicDevice implementations
        on windows.
        
        Use this base class if your target device is a
        window. Additionally to GraphicDeviceBase, this template
        provides an implementation of the awt::XWindowListener
        interface, to receive notifications about state changes of the
        associated window.

        @tpl Base
        Base class to use, most probably one of the
        WeakComponentImplHelperN templates with the appropriate
        interfaces. At least XGraphicDevice should be among them (why else
        would you use this template, then?). Base class must have an
        Base( const Mutex& ) constructor (like the
        WeakComponentImplHelperN templates have). As the very least,
        the base class must be derived from uno::XInterface, as some
        error reporting mechanisms rely on that.

        @tpl DeviceHelper
        Device helper implementation for the backend in question. This
        object will be held as a member of this template class, and
        basically gets forwarded all XGraphicDevice API calls that
        could not be handled generically.

        @tpl Mutex
        Lock strategy to use. Defaults to using the
        OBaseMutex-provided lock.  Every time one of the methods is
        entered, an object of type Mutex is created with m_aMutex as
        the sole parameter, and destroyed again when the method scope
        is left.

        @tpl UnambiguousBase
        Optional unambiguous base class for XInterface of Base. It's
        sometimes necessary to specify this parameter, e.g. if Base
        derives from multiple UNO interface (were each provides its
        own version of XInterface, making the conversion ambiguous)
     */
    template< class Base, 
              class DeviceHelper, 
              class Mutex=::osl::MutexGuard, 
              class UnambiguousBase=::com::sun::star::uno::XInterface > class BufferedGraphicDeviceBase : 
        public GraphicDeviceBase< Base, DeviceHelper, Mutex, UnambiguousBase >
    {
    public:
        typedef GraphicDeviceBase< Base, DeviceHelper, Mutex, UnambiguousBase >	BaseType;
        typedef BufferedGraphicDeviceBase OurType;
        typedef Mutex MutexType;

        BufferedGraphicDeviceBase() :
            mxWindow(),
            maBounds(),
            mbIsVisible( false ),
            mbIsTopLevel( false )
        {
            BaseType::maPropHelper.addProperties( PropertySetHelper::MakeMap
                                                  ("Window",
                                                   boost::bind(&OurType::getXWindow,
                                                               this)));
        }

        // XGraphicDevice
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XBufferController > SAL_CALL getBufferController(  ) throw (::com::sun::star::uno::RuntimeException)
        {
            return this;
        }

        // XBufferController
        virtual ::sal_Int32 SAL_CALL createBuffers( ::sal_Int32 nBuffers ) throw (::com::sun::star::lang::IllegalArgumentException, 
                                                                                  ::com::sun::star::uno::RuntimeException)
        {
            tools::verifyRange( nBuffers, (sal_Int32)1 );

            MutexType aGuard( BaseType::m_aMutex );

            return BaseType::maDeviceHelper.createBuffers( nBuffers );
        }

        virtual void SAL_CALL destroyBuffers(  ) throw (::com::sun::star::uno::RuntimeException)
        {
            MutexType aGuard( BaseType::m_aMutex );

            BaseType::maDeviceHelper.destroyBuffers();
        }

        virtual ::sal_Bool SAL_CALL showBuffer( ::sal_Bool bUpdateAll ) throw (::com::sun::star::uno::RuntimeException)
        {
            MutexType aGuard( BaseType::m_aMutex );

            return BaseType::maDeviceHelper.showBuffer( mbIsVisible, bUpdateAll );
        }

        virtual ::sal_Bool SAL_CALL switchBuffer( ::sal_Bool bUpdateAll ) throw (::com::sun::star::uno::RuntimeException)
        {
            MutexType aGuard( BaseType::m_aMutex );

            return BaseType::maDeviceHelper.switchBuffer( mbIsVisible, bUpdateAll );
        }


        /** Set corresponding canvas window

            Use this method to set the window this canvas displays
            on. Comes in handy when the canvas needs to adapt size or
            output position to the changing window.

            Whenever the bounds of the window change, <code>void
            notifySizeUpdate( const awt::Rectangle& rBounds )</code>
            is called, with rBounds the window bound rect relative to
            the frame window.
         */
        void setWindow( const ::com::sun::star::uno::Reference< 
                              ::com::sun::star::awt::XWindow2 >& rWindow )
        {
            if( mxWindow.is() )
                mxWindow->removeWindowListener( this );

            mxWindow = rWindow;

            if( mxWindow.is() )
            {
                mbIsVisible = mxWindow->isVisible();
                mbIsTopLevel = 
                    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTopWindow >(
                        mxWindow,
                        ::com::sun::star::uno::UNO_QUERY ).is();

                maBounds = transformBounds( mxWindow->getPosSize() );
                mxWindow->addWindowListener( this );
            }
        }

        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow2 > getWindow() const
        {
            return mxWindow;
        }
        
        ::com::sun::star::uno::Any getXWindow() const
        {
            return ::com::sun::star::uno::makeAny(mxWindow);
        }

#if defined __SUNPRO_CC
        using Base::disposing;
#endif
        virtual void SAL_CALL disposing()
        {
            typename BaseType::MutexType aGuard( BaseType::m_aMutex );

            if( mxWindow.is() )
            {
                mxWindow->removeWindowListener(this);
                mxWindow.clear();
            }

            // pass on to base class
            BaseType::disposing();
        }

        ::com::sun::star::awt::Rectangle transformBounds( const ::com::sun::star::awt::Rectangle& rBounds )
        {        
            // notifySizeUpdate's bounds are relative to the toplevel
            // window
            if( !mbIsTopLevel )
                return tools::getAbsoluteWindowRect(
                    rBounds,
                    mxWindow );
            else
                return ::com::sun::star::awt::Rectangle( 0,0,rBounds.Width,rBounds.Height );
        }

        void boundsChanged( const ::com::sun::star::awt::WindowEvent& e )
        {
            typename BaseType::MutexType aGuard( BaseType::m_aMutex );

            const ::com::sun::star::awt::Rectangle& rNewBounds(
                transformBounds(
                    ::com::sun::star::awt::Rectangle( e.X,
                                                      e.Y,
                                                      e.Width,
                                                      e.Height )));

            if( rNewBounds.X != maBounds.X ||
                rNewBounds.Y != maBounds.Y ||
                rNewBounds.Width != maBounds.Width ||
                rNewBounds.Height != maBounds.Height )
            {
                maBounds = rNewBounds;
                BaseType::maDeviceHelper.notifySizeUpdate( maBounds );
            }
        }

        // XWindowListener
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException)
        {
            typename BaseType::MutexType aGuard( BaseType::m_aMutex );

            if( Source.Source == mxWindow )
                mxWindow.clear();
        }

        virtual void SAL_CALL windowResized( const ::com::sun::star::awt::WindowEvent& e ) throw (::com::sun::star::uno::RuntimeException)
        {
            boundsChanged( e );
        }

        virtual void SAL_CALL windowMoved( const ::com::sun::star::awt::WindowEvent& e ) throw (::com::sun::star::uno::RuntimeException)
        {
            boundsChanged( e );
        }

        virtual void SAL_CALL windowShown( const ::com::sun::star::lang::EventObject& ) throw (::com::sun::star::uno::RuntimeException)
        {
            typename BaseType::MutexType aGuard( BaseType::m_aMutex );

            mbIsVisible = true;
        }

        virtual void SAL_CALL windowHidden( const ::com::sun::star::lang::EventObject& ) throw (::com::sun::star::uno::RuntimeException)
        {
            typename BaseType::MutexType aGuard( BaseType::m_aMutex );

            mbIsVisible = false;
        }

    protected:
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow2 >  mxWindow;

        /// Current bounds of the owning Window
	    ::com::sun::star::awt::Rectangle                                     maBounds;

        /// True, if the window this canvas is contained in, is visible
        bool                                                                 mbIsVisible;

    private:
        /// True, if the window this canvas is contained in, is a toplevel window
        bool                                                                 mbIsTopLevel;
    };
}

#endif /* INCLUDED_CANVAS_BUFFEREDGRAPHICDEVICEBASE_HXX */
