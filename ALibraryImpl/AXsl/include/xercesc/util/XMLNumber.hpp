/*
 * Copyright 2001-2004 The Apache Software Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Id: XMLNumber.hpp,v 1.1.1.1 2006/04/07 01:16:01 achacha Exp $
 */

#ifndef XMLNUMBER_HPP
#define XMLNUMBER_HPP

#include <xercesc/internal/XSerializable.hpp>
#include <xercesc/util/XMemory.hpp>

XERCES_CPP_NAMESPACE_BEGIN

class XMLUTIL_EXPORT XMLNumber : public XSerializable, public XMemory
{
public:

    enum
    {
        LESS_THAN     = -1,
        EQUAL         = 0,
        GREATER_THAN  = 1,
        INDETERMINATE = 2
    };

    enum NumberType {
        Float,
        Double,
        BigDecimal,
        DateTime,
        UnKnown
    };

    virtual ~XMLNumber();

    /**
     *  Deprecated: please use getRawData
     *
     *  Return string representation of the decimal value.
     *  A decimal point will be included as necessary,
     *  the caller of this method is responsible for the
     *  de-allocation of the memory.
     */
    virtual XMLCh*     toString() const = 0;
    
	/**
	 *  Return string representation of the decimal value.
     *  A decimal point will be included as necessary.
     *  Similar to toString above, but the internal buffer is
     *  returned directly, user is not required to delete
     *  the returned buffer
	 */
    virtual XMLCh*     getRawData() const = 0;    

    /**
     *  Return the original and converted value of the original data.
     *  (applicable to double/float)
     *
     *  The internal buffer is returned directly, user is not required 
     *  to delete the returned buffer
     */
    virtual const XMLCh*     getFormattedString() const = 0;    

	/**
	 * Returns the sign of this number
     *
     * -1   negative
     *  0   zero
     *  1   positive
	 *
	 */
    virtual int        getSign() const = 0;

    /***
     * Support for Serialization/De-serialization
     ***/
    DECL_XSERIALIZABLE(XMLNumber)

    static XMLNumber*          loadNumber(XMLNumber::NumberType   numType
                                        , XSerializeEngine&       serEng);

protected:

    XMLNumber();
    XMLNumber(const XMLNumber&);

private:
    // -----------------------------------------------------------------------
    //  Unimplemented operators
    // -----------------------------------------------------------------------
    XMLNumber& operator=(const XMLNumber&);
};

XERCES_CPP_NAMESPACE_END

#endif
