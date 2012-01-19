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

#ifndef INCLUDED_DRAWINGLAYER_ATTRIBUTE_FILLHATCHATTRIBUTE_HXX
#define INCLUDED_DRAWINGLAYER_ATTRIBUTE_FILLHATCHATTRIBUTE_HXX

#include <drawinglayer/drawinglayerdllapi.h>
//////////////////////////////////////////////////////////////////////////////
// predefines

namespace basegfx {
    class BColor;
}

namespace drawinglayer { namespace attribute {
    class ImpFillHatchAttribute;
}}

//////////////////////////////////////////////////////////////////////////////
// declarations

namespace drawinglayer
{
	namespace attribute
	{
		enum HatchStyle
		{
			HATCHSTYLE_SINGLE, 
			HATCHSTYLE_DOUBLE, 
			HATCHSTYLE_TRIPLE
		};
	} // end of namespace attribute
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
	namespace attribute
	{
		class DRAWINGLAYER_DLLPUBLIC FillHatchAttribute
		{
        private:
            ImpFillHatchAttribute*              mpFillHatchAttribute;

		public:
            /// constructors/assignmentoperator/destructor
			FillHatchAttribute(
			    HatchStyle eStyle, 
                double fDistance, 
                double fAngle, 
                const basegfx::BColor& rColor, 
                bool bFillBackground);
			FillHatchAttribute();
			FillHatchAttribute(const FillHatchAttribute& rCandidate);
			FillHatchAttribute& operator=(const FillHatchAttribute& rCandidate);
			~FillHatchAttribute();

            // checks if the incarnation is default constructed
            bool isDefault() const;

            // compare operator
			bool operator==(const FillHatchAttribute& rCandidate) const;

			// data read access
			HatchStyle getStyle() const;
			double getDistance() const;
			double getAngle() const;
			const basegfx::BColor& getColor() const;
			bool isFillBackground() const;
		};
	} // end of namespace attribute
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_DRAWINGLAYER_ATTRIBUTE_FILLHATCHATTRIBUTE_HXX

// eof
