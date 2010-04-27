/* IcedTeaPluginUtils.cc

   Copyright (C) 2009  Red Hat

This file is part of IcedTea.

IcedTea is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

IcedTea is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with IcedTea; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

#include "IcedTeaNPPlugin.h"
#include "IcedTeaScriptablePluginObject.h"
#include "IcedTeaPluginUtils.h"

/**
 * Misc. utility functions used by the plugin
 */

/***********************************************
 * Begin IcedTeaPluginUtilities implementation *
************************************************/

// Initialize static variables
int IcedTeaPluginUtilities::reference = 0;
pthread_mutex_t IcedTeaPluginUtilities::reference_mutex = PTHREAD_MUTEX_INITIALIZER;
std::map<void*, NPP>* IcedTeaPluginUtilities::instance_map = new std::map<void*, NPP>();
std::map<std::string, NPObject*>* IcedTeaPluginUtilities::object_map = new std::map<std::string, NPObject*>();

/**
 * Given a context number, constructs a message prefix to send to Java
 *
 * @param context The context of the request
 * @return The string prefix (allocated on heap)
 */

void
IcedTeaPluginUtilities::constructMessagePrefix(int context, std::string *result)
{
	std::string context_str = std::string();

	itoa(context, &context_str);

	result->append("context ");
	result->append(context_str);
	result->append(" reference -1");

}

/**
 * Given a context number, and reference number, constructs a message prefix to
 * send to Java
 *
 * @param context The context of the request
 * @param rerefence The reference number of the request
 * @param result The message
 */

void
IcedTeaPluginUtilities::constructMessagePrefix(int context, int reference, std::string* result)
{
    // Until security is implemented, use file:// source for _everything_

	std::string context_str = std::string();
	std::string reference_str = std::string();

	itoa(context, &context_str);
	itoa(reference, &reference_str);

	*result += "context ";
	result->append(context_str);
	*result += " reference ";
	result->append(reference_str);
}

/**
 * Given a context number, reference number, and source location, constructs
 * a message prefix to send to Java
 *
 * @param context The context of the request
 * @param rerefence The reference number of the request
 * @param address The address for the script that made the request
 * @param result The message
 */

void
IcedTeaPluginUtilities::constructMessagePrefix(int context, int reference,
		                                       std::string address,
		                                       std::string* result)
{
	std::string context_str = std::string();
	std::string reference_str = std::string();

	itoa(context, &context_str);
	itoa(reference, &reference_str);

	*result += "context ";
	result->append(context_str);
	*result += " reference ";
	result->append(reference_str);

	if (address.length() > 0)
	{
	    *result += " src ";
        result->append(address);
	}
}

/**
 * Returns a string representation of a void pointer
 *
 * @param id The pointer
 * @param result The string representation
 */

void
IcedTeaPluginUtilities::JSIDToString(void* id, std::string* result)
{

	char* id_str = (char*) malloc(sizeof(char)*20); // max = long long = 8446744073709551615 == 19 chars

	if (sizeof(void*) == sizeof(long long))
	{
		sprintf(id_str, "%llu", id);
	}
	else
	{
		sprintf(id_str, "%lu", id); // else use long
	}

	result->append(id_str);

	PLUGIN_DEBUG_2ARG("Converting pointer %p to %s\n", id, id_str);
	free(id_str);
}

/**
 * Returns a void pointer from a string representation
 *
 * @param id_str The string representation
 * @return The pointer
 */

void*
IcedTeaPluginUtilities::stringToJSID(std::string id_str)
{
	void* ptr;
	if (sizeof(void*) == sizeof(long long))
	{
		PLUGIN_DEBUG_2ARG("Casting (long long) \"%s\" -- %llu\n", id_str.c_str(), strtoull(id_str.c_str(), NULL, 0));
		ptr = reinterpret_cast <void*> ((unsigned long long) strtoull(id_str.c_str(), NULL, 0));
	} else
	{
		PLUGIN_DEBUG_2ARG("Casting (long) \"%s\" -- %lu\n", id_str.c_str(), strtoul(id_str.c_str(), NULL, 0));
		ptr = reinterpret_cast <void*> ((unsigned long)  strtoul(id_str.c_str(), NULL, 0));
	}

	PLUGIN_DEBUG_1ARG("Casted: %p\n", ptr);

	return ptr;
}

/**
 * Increments the global reference number and returns it.
 *
 * This function is thread-safe.
 */
int
IcedTeaPluginUtilities::getReference()
{
	pthread_mutex_lock(&reference_mutex);

	// If we are nearing the max, reset
	if (reference > 0x7FFFFFFF - 10) {
	    reference = 0;
	}

	reference++;
	pthread_mutex_unlock(&reference_mutex);

	return reference;
}

/**
 * Decrements the global reference number.
 *
 * This function is thread-safe.
 */
void
IcedTeaPluginUtilities::releaseReference()
{
    // do nothing for now
}

/**
 * Converts integer to char*
 *
 * @param i The integer to convert to ascii
 * @param result The resulting string
 */
void
IcedTeaPluginUtilities::itoa(int i, std::string* result)
{
	// largest possible integer is 10 digits long
	char* int_str = (char*) malloc(sizeof(char)*11);
	sprintf(int_str, "%d", i);
	result->append(int_str);

	free(int_str);
}

/**
 * Frees memory from a string* vector
 *
 * The vector deconstructor will only delete string pointers upon being
 * called. This function frees the associated string memory as well.
 *
 * @param v The vector whose strings are to be freed
 */
void
IcedTeaPluginUtilities::freeStringPtrVector(std::vector<std::string*>* v)
{
	if (v)
	{
		for (int i=0; i < v->size(); i++)
			delete v->at(i);

		delete v;
	}

}

/**
 * Given a string, splits it on the given delimiters.
 *
 * @param str The string to split
 * @param The delimiters to split on
 * @return A string vector containing the aplit components
 */

std::vector<std::string>*
IcedTeaPluginUtilities::strSplit(const char* str, const char* delim)
{
	std::vector<std::string>* v = new std::vector<std::string>();
	v->reserve(strlen(str)/2);
	char* copy;

	// Tokening is done on a copy
	copy = (char*) malloc (sizeof(char)*strlen(str) + 1);
	strcpy(copy, str);

	char* tok_ptr;
	tok_ptr = strtok (copy, delim);

	while (tok_ptr != NULL)
	{
		std::string* s = new std::string();
		s->append(tok_ptr);
		v->push_back(*s);
		tok_ptr = strtok (NULL, " ");
	}

	return v;
}

/**
 * Given a unicode byte array, converts it to a UTF8 string
 *
 * The actual contents in the array may be surrounded by other data.
 *
 * e.g. with length 5, begin = 3,
 * unicode_byte_array = "37 28 5 48 45 4c 4c 4f 9e 47":
 *
 * We'd start at 3 i.e. "48" and go on for 5 i.e. upto and including "4f".
 * So we convert "48 45 4c 4c 4f" which is "hello"
 *
 * @param length The length of the string
 * @param begin Where in the array to begin conversion
 * @param result_unicode_str The return variable in which the
 *        converted string is placed
 */

void
IcedTeaPluginUtilities::getUTF8String(int length, int begin, std::vector<std::string>* unicode_byte_array, std::string* result_unicode_str)
{
	result_unicode_str->clear();
	result_unicode_str->reserve(unicode_byte_array->size()/2);
	for (int i = begin; i < begin+length; i++)
	    result_unicode_str->push_back((char) strtol(unicode_byte_array->at(i).c_str(), NULL, 16));

	PLUGIN_DEBUG_2ARG("Converted UTF-8 string: %s. Length=%d\n", result_unicode_str->c_str(), result_unicode_str->length());
}

/**
 * Given a UTF8 string, converts it to a space delimited string of hex characters
 *
 * The first element in the return array is the length of the string
 *
 * e.g. "hello" would convert to: "5 48 45 4c 4c 4f"
 *
 * @param str The string to convert
 * @param urt_str The result
 */

void
IcedTeaPluginUtilities::convertStringToUTF8(std::string* str, std::string* utf_str)
{
	std::ostringstream ostream;

	std::string length = std::string();
	itoa(str->length(), &length);

	ostream << length;

	// UTF-8 characters are 4-bytes max + space + '\0'
	char* hex_value = (char*) malloc(sizeof(char)*10);

	for (int i = 0; i < str->length(); i++)
	{
		sprintf(hex_value, " %hx", str->at(i));
		ostream << hex_value;
	}

	utf_str->clear();
	*utf_str = ostream.str();

	free(hex_value);
	PLUGIN_DEBUG_2ARG("Converted %s to UTF-8 string %s\n", str->c_str(), utf_str->c_str());
}

/**
 * Given a unicode byte array, converts it to a UTF16LE/UCS-2 string
 *
 * This works in a manner similar to getUTF8String, except that it reads 2
 * slots for each byte.
 *
 * @param length The length of the string
 * @param begin Where in the array to begin conversion
 * @param result_unicode_str The return variable in which the
 *        converted string is placed
 */
void
IcedTeaPluginUtilities::getUTF16LEString(int length, int begin, std::vector<std::string>* unicode_byte_array, std::wstring* result_unicode_str)
{

	wchar_t c;

	if (plugin_debug) printf("Converted UTF-16LE string: ");

	result_unicode_str->clear();
	for (int i = begin; i < begin+length; i+=2)
	{
		int low = strtol(unicode_byte_array->at(i).c_str(), NULL, 16);
		int high = strtol(unicode_byte_array->at(i+1).c_str(), NULL, 16);

        c = ((high << 8) | low);

        if ((c >= 'a' && c <= 'z') ||
        	(c >= 'A' && c <= 'Z') ||
        	(c >= '0' && c <= '9'))
        {
        	if (plugin_debug) printf("%c", c);
        }

        result_unicode_str->push_back(c);
	}

	// not routing via debug print macros due to wide-string issues
	if (plugin_debug) printf(". Length=%d\n", result_unicode_str->length());
}

/*
 * Prints the given string vector (if debug is true)
 *
 * @param prefix The prefix to print before printing the vector contents
 * @param cv The string vector whose contents are to be printed
 */
void
IcedTeaPluginUtilities::printStringVector(const char* prefix, std::vector<std::string>* str_vector)
{

        // This is a CPU intensive function. Run only if debugging
        if (!plugin_debug)
            return;

	std::string* str = new std::string();
	*str += "{ ";
	for (int i=0; i < str_vector->size(); i++)
	{
		*str += str_vector->at(i);

		if (i != str_vector->size() - 1)
			*str += ", ";
	}

	*str += " }";

	PLUGIN_DEBUG_2ARG("%s %s\n", prefix, str->c_str());

	delete str;
}

const gchar*
IcedTeaPluginUtilities::getSourceFromInstance(NPP instance)
{
    // At the moment, src cannot be securely fetched via NPAPI
    // See:
    // http://www.mail-archive.com/chromium-dev@googlegroups.com/msg04872.html

    // Since we use the insecure window.location.href attribute to compute
    // source, we cannot use it to make security decisions. Therefore,
    // instance associated source will always return empty

    //GCJPluginData* data = (GCJPluginData*) instance->pdata;
    //return (data->source) ? data->source : "";

    return "http://null.null";
}

/**
 * Stores a window pointer <-> instance mapping
 *
 * @param member_ptr The pointer key
 * @param instance The instance to associate with this pointer
 */

void
IcedTeaPluginUtilities::storeInstanceID(void* member_ptr, NPP instance)
{
    PLUGIN_DEBUG_2ARG("Storing instance %p with key %p\n", instance, member_ptr);
    instance_map->insert(std::make_pair(member_ptr, instance));
}

/**
 * Removes a window pointer <-> instance mapping
 *
 * @param member_ptr The key to remove
 */

void
IcedTeaPluginUtilities::removeInstanceID(void* member_ptr)
{
    PLUGIN_DEBUG_1ARG("Removing key %p from instance map\n", member_ptr);
    instance_map->erase(member_ptr);
}

/**
 * Removes all mappings to a given instance, and all associated objects
 */
void
IcedTeaPluginUtilities::invalidateInstance(NPP instance)
{
    PLUGIN_DEBUG_1ARG("Invalidating instance %p\n", instance);

    std::map<void*,NPP>::iterator iterator;

    for (iterator = instance_map->begin(); iterator != instance_map->end(); iterator++)
    {
        if ((*iterator).second == instance)
        {
            instance_map->erase((*iterator).first);
        }
    }
}

/**
 * Given the window pointer, returns the instance associated with it
 *
 * @param member_ptr The pointer key
 * @return The associated instance
 */

NPP
IcedTeaPluginUtilities::getInstanceFromMemberPtr(void* member_ptr)
{

    NPP instance = NULL;
    PLUGIN_DEBUG_1ARG("getInstanceFromMemberPtr looking for %p\n", member_ptr);

    std::map<void*, NPP>::iterator iterator = instance_map->find(member_ptr);

    if (iterator != instance_map->end())
    {
        instance = instance_map->find(member_ptr)->second;
        PLUGIN_DEBUG_2ARG("getInstanceFromMemberPtr found %p. Instance = %p\n", member_ptr, instance);
    }

    return instance;
}

/**
 * Given a java id key ('classid:instanceid'), returns the associated valid NPObject, if any
 *
 * @param key the key
 * @return The associated active NPObject, NULL otherwise
 */

NPObject*
IcedTeaPluginUtilities::getNPObjectFromJavaKey(std::string key)
{

    NPObject* object = NULL;
    PLUGIN_DEBUG_1ARG("getNPObjectFromJavaKey looking for %s\n", key.c_str());

    std::map<std::string, NPObject*>::iterator iterator = object_map->find(key);

    if (iterator != object_map->end())
    {
        NPObject* mapped_object = object_map->find(key)->second;

        if (getInstanceFromMemberPtr(mapped_object) != NULL)
        {
            object = mapped_object;
            PLUGIN_DEBUG_2ARG("getNPObjectFromJavaKey found %s. NPObject = %p\n", key.c_str(), object);
        }
    }

    return object;
}

/**
 * Stores a java id key <-> NPObject mapping
 *
 * @param key The Java ID Key
 * @param object The object to map to
 */

void
IcedTeaPluginUtilities::storeObjectMapping(std::string key, NPObject* object)
{
    PLUGIN_DEBUG_2ARG("Storing object %p with key %s\n", object, key.c_str());
    object_map->insert(std::make_pair(key, object));
}

/**
 * Removes a java id key <-> NPObject mapping
 *
 * @param key The key to remove
 */

void
IcedTeaPluginUtilities::removeObjectMapping(std::string key)
{
    PLUGIN_DEBUG_1ARG("Removing key %s from object map\n", key.c_str());
    object_map->erase(key);
}

/*
 * Similar to printStringVector, but takes a vector of string pointers instead
 *
 * @param prefix The prefix to print before printing the vector contents
 * @param cv The string* vector whose contents are to be printed
 */

void
IcedTeaPluginUtilities::printStringPtrVector(const char* prefix, std::vector<std::string*>* str_ptr_vector)
{
        // This is a CPU intensive function. Run only if debugging
        if (!plugin_debug)
            return;

	std::string* str = new std::string();
	*str += "{ ";
	for (int i=0; i < str_ptr_vector->size(); i++)
	{
		*str += *(str_ptr_vector->at(i));

		if (i != str_ptr_vector->size() - 1)
			*str += ", ";
	}

	*str += " }";

	PLUGIN_DEBUG_2ARG("%s %s\n", prefix, str->c_str());

	delete str;
}

void
IcedTeaPluginUtilities::printNPVariant(NPVariant variant)
{
    // This is a CPU intensive function. Run only if debugging
    if (!plugin_debug)
        return;

    if (NPVARIANT_IS_VOID(variant))
    {
    	PLUGIN_DEBUG_1ARG("VOID %d\n", variant);
    }
    else if (NPVARIANT_IS_NULL(variant))
    {
    	PLUGIN_DEBUG_1ARG("NULL\n", variant);
    }
    else if (NPVARIANT_IS_BOOLEAN(variant))
    {
    	PLUGIN_DEBUG_1ARG("BOOL: %d\n", NPVARIANT_TO_BOOLEAN(variant));
    }
    else if (NPVARIANT_IS_INT32(variant))
    {
    	PLUGIN_DEBUG_1ARG("INT32: %d\n", NPVARIANT_TO_INT32(variant));
    }
    else if (NPVARIANT_IS_DOUBLE(variant))
    {
    	PLUGIN_DEBUG_1ARG("DOUBLE: %f\n", NPVARIANT_TO_DOUBLE(variant));
    }
    else if (NPVARIANT_IS_STRING(variant))
    {
#if MOZILLA_VERSION_COLLAPSED < 1090200
    	PLUGIN_DEBUG_1ARG("STRING: %s\n", NPVARIANT_TO_STRING(variant).utf8characters);
#else
    	PLUGIN_DEBUG_1ARG("STRING: %s\n", NPVARIANT_TO_STRING(variant).UTF8Characters);
#endif
    }
    else
    {
    	PLUGIN_DEBUG_1ARG("OBJ: %p\n", NPVARIANT_TO_OBJECT(variant));
    }
}

void
IcedTeaPluginUtilities::NPVariantToString(NPVariant variant, std::string* result)
{
	char* str = (char*) malloc(sizeof(char)*32); // enough for everything except string

    if (NPVARIANT_IS_VOID(variant))
    {
        sprintf(str, "%p", variant);
    }
    else if (NPVARIANT_IS_NULL(variant))
    {
    	sprintf(str, "NULL");
    }
    else if (NPVARIANT_IS_BOOLEAN(variant))
    {
    	if (NPVARIANT_TO_BOOLEAN(variant))
    		sprintf(str, "true");
    	else
    		sprintf(str, "false");
    }
    else if (NPVARIANT_IS_INT32(variant))
    {
    	sprintf(str, "%d", NPVARIANT_TO_INT32(variant));
    }
    else if (NPVARIANT_IS_DOUBLE(variant))
    {
    	sprintf(str, "%f", NPVARIANT_TO_DOUBLE(variant));;
    }
    else if (NPVARIANT_IS_STRING(variant))
    {
    	free(str);
#if MOZILLA_VERSION_COLLAPSED < 1090200
    	str = (char*) malloc(sizeof(char)*NPVARIANT_TO_STRING(variant).utf8length);
    	sprintf(str, "%s", NPVARIANT_TO_STRING(variant).utf8characters);
#else
        str = (char*) malloc(sizeof(char)*NPVARIANT_TO_STRING(variant).UTF8Length);
        sprintf(str, "%s", NPVARIANT_TO_STRING(variant).UTF8Characters);
#endif
    }
    else
    {
        sprintf(str, "[Object %p]", variant);
    }

    result->append(str);
    free(str);
}

bool
IcedTeaPluginUtilities::javaResultToNPVariant(NPP instance,
                                              std::string* java_value,
                                              NPVariant* variant)
{
    JavaRequestProcessor java_request = JavaRequestProcessor();
    JavaResultData* java_result;

    if (java_value->find("literalreturn") == 0)
    {
        // 'literalreturn ' == 14 to skip
        std::string value = java_value->substr(14);

        // VOID/BOOLEAN/NUMBER

        if (value == "void")
        {
            PLUGIN_DEBUG_0ARG("Method call returned void\n");
            VOID_TO_NPVARIANT(*variant);
        } else if (value == "null")
        {
            PLUGIN_DEBUG_0ARG("Method call returned null\n");
            NULL_TO_NPVARIANT(*variant);
        }else if (value == "true")
        {
            PLUGIN_DEBUG_0ARG("Method call returned a boolean (true)\n");
            BOOLEAN_TO_NPVARIANT(true, *variant);
        } else if (value == "false")
        {
            PLUGIN_DEBUG_0ARG("Method call returned a boolean (false)\n");
            BOOLEAN_TO_NPVARIANT(false, *variant);
        } else
        {
            double d = strtod(value.c_str(), NULL);

            // See if it is convertible to int
            if (value.find(".") != std::string::npos ||
                d < -(0x7fffffffL - 1L) ||
                d > 0x7fffffffL)
            {
                PLUGIN_DEBUG_1ARG("Method call returned a double %f\n", d);
                DOUBLE_TO_NPVARIANT(d, *variant);
            } else
            {
                int32_t i = (int32_t) d;
                PLUGIN_DEBUG_1ARG("Method call returned an int %d\n", i);
                INT32_TO_NPVARIANT(i, *variant);
            }
        }
    } else {
        // Else this is a complex java object

        // To keep code a little bit cleaner, we create variables with proper descriptive names
        std::string return_obj_instance_id = std::string();
        std::string return_obj_class_id = std::string();
        std::string return_obj_class_name = std::string();
        return_obj_instance_id.append(*java_value);

        // Find out the class name first, because string is a special case
        java_result = java_request.getClassName(return_obj_instance_id);

        if (java_result->error_occurred)
        {
            return false;
        }

        return_obj_class_name.append(*(java_result->return_string));

        if (return_obj_class_name == "java.lang.String")
        {
            // String is a special case as NPVariant can handle it directly
            java_result = java_request.getString(return_obj_instance_id);

            if (java_result->error_occurred)
            {
                return false;
            }

            // needs to be on the heap
            NPUTF8* return_str = (NPUTF8*) malloc(sizeof(NPUTF8)*java_result->return_string->size() + 1);
            strcpy(return_str, java_result->return_string->c_str());

            PLUGIN_DEBUG_1ARG("Method call returned a string: \"%s\"\n", return_str);
            STRINGZ_TO_NPVARIANT(return_str, *variant);

        } else {

            // Else this is a regular class. Reference the class object so
            // we can construct an NPObject with it and the instance
            java_result = java_request.getClassID(return_obj_instance_id);

            if (java_result->error_occurred)
            {
                return false;
            }

            return_obj_class_id.append(*(java_result->return_string));

            NPObject* obj;

            if (return_obj_class_name.find('[') == 0) // array
                obj = IcedTeaScriptableJavaPackageObject::get_scriptable_java_object(
                                instance,
                                return_obj_class_id, return_obj_instance_id, true);
            else
                obj = IcedTeaScriptableJavaPackageObject::get_scriptable_java_object(
                                                instance,
                                                return_obj_class_id, return_obj_instance_id, false);

            OBJECT_TO_NPVARIANT(obj, *variant);
        }
    }

    return true;
}

bool
IcedTeaPluginUtilities::isObjectJSArray(NPP instance, NPObject* object)
{

    NPVariant constructor_v = NPVariant();
    NPIdentifier constructor_id = browser_functions.getstringidentifier("constructor");
    browser_functions.getproperty(instance, object, constructor_id, &constructor_v);
    IcedTeaPluginUtilities::printNPVariant(constructor_v);

    // void constructor => not an array
    if (NPVARIANT_IS_VOID(constructor_v))
        return false;

    NPObject* constructor = NPVARIANT_TO_OBJECT(constructor_v);

    NPVariant constructor_str;
    NPIdentifier toString = browser_functions.getstringidentifier("toString");
    browser_functions.invoke(instance, constructor, toString, NULL, 0, &constructor_str);
    IcedTeaPluginUtilities::printNPVariant(constructor_str);

    std::string constructor_name = std::string();

#if MOZILLA_VERSION_COLLAPSED < 1090200
    constructor_name.append(NPVARIANT_TO_STRING(constructor_str).utf8characters);
#else
    constructor_name.append(NPVARIANT_TO_STRING(constructor_str).UTF8Characters);
#endif

    PLUGIN_DEBUG_1ARG("Constructor for NPObject is %s\n", constructor_name.c_str());

    return constructor_name.find("function Array") == 0;
}

void
IcedTeaPluginUtilities::decodeURL(const gchar* url, gchar** decoded_url)
{

    PLUGIN_DEBUG_2ARG("GOT URL: %s -- %s\n", url, *decoded_url);
    int length = strlen(url);
    for (int i=0; i < length; i++)
    {
        if (url[i] == '%' && i < length - 2)
        {
            unsigned char code1 = (unsigned char) url[i+1];
            unsigned char code2 = (unsigned char) url[i+2];

            if (!IS_VALID_HEX(&code1) || !IS_VALID_HEX(&code2))
                continue;

            // Convert hex value to integer
            int converted1 = HEX_TO_INT(&code1);
            int converted2 = HEX_TO_INT(&code2);

            // bitshift 4 to simulate *16
            int value = (converted1 << 4) + converted2;
            char decoded = value;

            strncat(*decoded_url, &decoded, 1);

            i += 2;
        } else
        {
            strncat(*decoded_url, &url[i], 1);
        }
    }

    PLUGIN_DEBUG_1ARG("SENDING URL: %s\n", *decoded_url);
}

/******************************************
 * Begin JavaMessageSender implementation *
 ******************************************
 *
 * This implementation is very simple and is therefore folded into this file
 * rather than a new one.
 */

/**
 * Sends to the Java side
 *
 * @param message The message to send.
 * @param returns whether the message was consumable (always true)
 */

bool
JavaMessageSender::newMessageOnBus(const char* message)
{
	char* msg = (char*) malloc(sizeof(char)*strlen(message) + 1);
	strcpy(msg, message);
	plugin_send_message_to_appletviewer(msg);

	free(msg);
	msg = NULL;

	// Always successful
	return true;
}

/***********************************
 * Begin MessageBus implementation *
 ***********************************/

/**
 * Constructor.
 *
 * Initializes the mutexes needed by the other functions.
 */
MessageBus::MessageBus()
{
	int ret;

	ret = pthread_mutex_init(&subscriber_mutex, NULL);

	if(ret)
		PLUGIN_DEBUG_1ARG("Error: Unable to initialize subscriber mutex: %d\n", ret);

	ret = pthread_mutex_init(&msg_queue_mutex, NULL);
	if(ret)
		PLUGIN_DEBUG_1ARG("Error: Unable to initialize message queue mutex: %d\n", ret);

	PLUGIN_DEBUG_2ARG("Mutexs %p and %p initialized\n", &subscriber_mutex, &msg_queue_mutex);
}

/**
 * Destructor.
 *
 * Destroy the mutexes initialized by the constructor.
 */

MessageBus::~MessageBus()
{
    PLUGIN_DEBUG_0ARG("MessageBus::~MessageBus\n");

	int ret;

	ret = pthread_mutex_destroy(&subscriber_mutex);
	if(ret)
		PLUGIN_DEBUG_1ARG("Error: Unable to destroy subscriber mutex: %d\n", ret);

	ret = pthread_mutex_destroy(&msg_queue_mutex);
	if(ret)
			PLUGIN_DEBUG_1ARG("Error: Unable to destroy message queue mutex: %d\n", ret);
}

/**
 * Adds the given BusSubscriber as a subscriber to self
 *
 * @param b The BusSubscriber to subscribe
 */
void
MessageBus::subscribe(BusSubscriber* b)
{
    // Applets may initialize in parallel. So lock before pushing.

	PLUGIN_DEBUG_2ARG("Subscribing %p to bus %p\n", b, this);
//    pthread_mutex_lock(&subscriber_mutex);
    subscribers.push_back(b);
//    pthread_mutex_unlock(&subscriber_mutex);
}

/**
 * Removes the given BusSubscriber from the subscriber list
 *
 * @param b The BusSubscriber to ubsubscribe
 */
void
MessageBus::unSubscribe(BusSubscriber* b)
{
    // Applets may initialize in parallel. So lock before pushing.

	PLUGIN_DEBUG_2ARG("Un-subscribing %p from bus %p\n", b, this);
//    pthread_mutex_lock(&subscriber_mutex);
    subscribers.remove(b);
//    pthread_mutex_unlock(&subscriber_mutex);
}

/**
 * Notifies all subscribers with the given message
 *
 * @param message The message to send to the subscribers
 */
void
MessageBus::post(const char* message)
{
	char* msg = (char*) malloc(sizeof(char)*strlen(message) + 1);
	bool message_consumed = false;

	// consumer frees this memory
	strcpy(msg, message);

	PLUGIN_DEBUG_1ARG("Trying to lock %p...\n", &msg_queue_mutex);
//    pthread_mutex_lock(&msg_queue_mutex);

    PLUGIN_DEBUG_1ARG("Message %s received on bus. Notifying subscribers.\n", msg);

    std::list<BusSubscriber*>::const_iterator i;
    for( i = subscribers.begin(); i != subscribers.end() && !message_consumed; ++i ) {
    	PLUGIN_DEBUG_2ARG("Notifying subscriber %p of %s\n", *i, msg);
    	message_consumed = ((BusSubscriber*) *i)->newMessageOnBus(msg);
    }

//    pthread_mutex_unlock(&msg_queue_mutex);

    if (!message_consumed)
    	PLUGIN_DEBUG_1ARG("Warning: No consumer found for message %s\n", msg);

    PLUGIN_DEBUG_1ARG("%p unlocked...\n", &msg_queue_mutex);
}