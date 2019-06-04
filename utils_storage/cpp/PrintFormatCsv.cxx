/*
 * (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include "PrintFormatCsv.hpp"

#include "dds_c/dds_c_xml.h"
#include "dds/core/xtypes/StructType.hpp"
#include "dds/core/xtypes/UnionType.hpp"
#include "dds/core/xtypes/MemberType.hpp"
#include "dds/core/xtypes/AliasType.hpp"

#include "Logger.hpp"

using namespace dds::core::xtypes;

#define RTI_PRINT_FORMAT_CSV_LOG_CURSOR(PF) \
    RTI_RECORDER_UTILS_LOG_MESSAGE( \
            rti::config::Verbosity::STATUS_ALL, \
            RTI_FUNCTION_NAME << ": " << *((PF).cursor()))

namespace rti { namespace recorder { namespace utils {

/*==============================================================================
 * Native CSV format print functions and definitions
 * ===========================================================================*/

class NativePrintFormatCsv {
public:

    static void print_top_level_beginning(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *,
            const char *,
            int)
    {
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        print_format.start_data_conversion();
    }

    static void print_top_level_ending(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *,
            const char *,
            int)
    {
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        print_format.pop_cursor();
    }

    static void print_complex_type_beginning(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *save_context,
            const char *name,
            int)
    {
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        RTI_PRINT_FORMAT_CSV_LOG_CURSOR(print_format);
        print_format.skip_cursor(name, save_context);
        print_format.push_cursor();
    }

    static void print_complex_type_ending(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *save_context,
            const char *,
            int)
    {
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        // for unions, we need to skip remaining fields
        print_format.skip_cursor_siblings(save_context);
        print_format.pop_cursor();
        RTI_PRINT_FORMAT_CSV_LOG_CURSOR(print_format);
        ++print_format.cursor();
    }

    static void print_primitive_type_beginning(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *save_context,
            const char *name,
            int)

    {
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        RTI_PRINT_FORMAT_CSV_LOG_CURSOR(print_format);
        print_format.skip_cursor(name, save_context);
        DDS_XMLHelper_save_freeform(
                save_context,
                "%s",
                PrintFormatCsv::COLUMN_SEPARATOR_DEFAULT().c_str());
    }

    static void print_primitive_type_ending(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *save_context,
            const char *name,
            int)
    {
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        RTI_PRINT_FORMAT_CSV_LOG_CURSOR(print_format);
        ++print_format.cursor();
    }

    static void print_array_beginning(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *save_context,
            const char *name,
            int)
    {

        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        RTI_PRINT_FORMAT_CSV_LOG_CURSOR(print_format);
        print_format.skip_cursor(name, save_context);
        print_format.push_cursor();
    }

    static void print_array_ending(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *save_context,
            const char *,
            int)
    {
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        // Skip as many columns as remaining elements in array
        print_format.skip_cursor_siblings(save_context);
        print_format.pop_cursor();
        RTI_PRINT_FORMAT_CSV_LOG_CURSOR(print_format);
        ++print_format.cursor();
    }

    static void print_complex_item_beginning(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *,
            DDS_UnsignedLong,
            int)
    {
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        RTI_PRINT_FORMAT_CSV_LOG_CURSOR(print_format);
        print_format.push_cursor();
    }

    static void print_complex_item_ending(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *save_context,
            DDS_UnsignedLong,
            int)
    {
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        // for unions, we need to skip remaining fields
        print_format.skip_cursor_siblings(save_context);
        print_format.pop_cursor();
        RTI_PRINT_FORMAT_CSV_LOG_CURSOR(print_format);
        ++print_format.cursor();
    }

    static void print_primitive_item_beginning(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *save_context,
            DDS_UnsignedLong,
            int)
    {
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        RTI_PRINT_FORMAT_CSV_LOG_CURSOR(print_format);
        DDS_XMLHelper_save_freeform(
                save_context,
                "%s",
                PrintFormatCsv::COLUMN_SEPARATOR_DEFAULT().c_str());
    }

    static void print_primitive_item_ending(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *save_context,
            DDS_UnsignedLong,
            int)
    {
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        RTI_PRINT_FORMAT_CSV_LOG_CURSOR(print_format);
        ++print_format.cursor();
    }

    static void print_array_item_beginning(
            struct DDS_PrintFormat *,
            struct RTIXMLSaveContext *,
            DDS_UnsignedLong,
            int)
    {
    }

    static void print_array_item_ending(
            struct DDS_PrintFormat *,
            struct RTIXMLSaveContext *,
            DDS_UnsignedLong,
            int)
    {
    }

    static void print_union_discriminator_beginning(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *save_context,
            int)
    {
        DDS_XMLHelper_save_freeform(
                save_context,
                "%s",
                PrintFormatCsv::COLUMN_SEPARATOR_DEFAULT().c_str());
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        RTI_PRINT_FORMAT_CSV_LOG_CURSOR(print_format);
    }

    static void print_union_discriminator_ending(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *,
            int)
    {
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        RTI_PRINT_FORMAT_CSV_LOG_CURSOR(print_format);
        ++print_format.cursor();
    }

    static void print_unset_optional_member_beginning(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *save_context,
            const char *,
            int)
    {
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        RTI_PRINT_FORMAT_CSV_LOG_CURSOR(print_format);
        print_format.skip_cursor(save_context);
    }

    static void print_unset_optional_member_ending(
            struct DDS_PrintFormat *self,
            struct RTIXMLSaveContext *,
            const char *,
            int)
    {
        PrintFormatCsv& print_format = PrintFormatCsv::from_native(self);
        RTI_PRINT_FORMAT_CSV_LOG_CURSOR(print_format);
        ++print_format.cursor();
    }

    static void get(DDS_PrintFormat& native)
    {
        /* CSV print functions */
        native.print_top_level_beginning =
                print_top_level_beginning;
        native.print_top_level_ending =
                print_top_level_ending;

        native.print_complex_type_beginning =
                print_complex_type_beginning;
        native.print_complex_type_ending =
                print_complex_type_ending;

        native.print_primitive_type_beginning =
                print_primitive_type_beginning;
        native.print_primitive_type_ending =
                print_primitive_type_ending;

        native.print_array_beginning =
                print_array_beginning;
        native.print_array_ending =
                print_array_ending;

        native.print_complex_item_beginning =
                print_complex_item_beginning;
        native.print_complex_item_ending =
                print_complex_item_ending;

        native.print_primitive_item_beginning =
                print_primitive_item_beginning;
        native.print_primitive_item_ending =
                print_primitive_item_ending;

        native.print_array_item_beginning =
                print_array_item_beginning;
        native.print_array_item_ending =
                print_array_item_ending;

        native.print_union_discriminator_beginning =
                print_union_discriminator_beginning;
        native.print_union_discriminator_ending =
                print_union_discriminator_ending;

        native.print_unset_optional_member_beginning =
                print_unset_optional_member_beginning;
        native.print_unset_optional_member_ending =
                print_unset_optional_member_ending;

        native.null_representation = "";
        native.element_separator = "";
        native.indent_representation = "";
    }


};

/*
 * --- PrintFormatCsvProperty -------------------------------------------------
 */

PrintFormatCsvProperty::PrintFormatCsvProperty() : 
    empty_member_value_rep_(""),
    enum_as_string_(false)
{

}

PrintFormatCsvProperty::PrintFormatCsvProperty(
        const PrintFormatCsvProperty& other) :
        empty_member_value_rep_(other.empty_member_value_rep_),
        enum_as_string_(other.enum_as_string_)
{

}

const std::string&
PrintFormatCsvProperty::empty_member_value_representation() const
{
    return empty_member_value_rep_;
}

PrintFormatCsvProperty& PrintFormatCsvProperty::empty_member_value_representation(
        const std::string& value)
{
    empty_member_value_rep_ = value;

    return *this;
}

bool PrintFormatCsvProperty::enum_as_string() const
{
    return enum_as_string_;
}

PrintFormatCsvProperty& PrintFormatCsvProperty::enum_as_string(bool the_enum_as_string)
{
    enum_as_string_ = the_enum_as_string;

    return *this;
}


/*
 * --- PrintFormatCsv ---------------------------------------------------------
 */


const PrintFormatCsvProperty& PrintFormatCsv::PROPERTY_DEFAULT()
{
    static PrintFormatCsvProperty property;
    static bool init = false;

    if (!init) {
        property.empty_member_value_representation(
                EMPTY_MEMBER_VALUE_REPRESENTATION_DEFAULT());
        property.enum_as_string(true);
        init = true;
    }

    return property;
}


const std::string& PrintFormatCsv::COLUMN_SEPARATOR_DEFAULT()
{
    static std::string value = ",";
    return value;
}

const std::string& PrintFormatCsv::EMPTY_MEMBER_VALUE_REPRESENTATION_DEFAULT()
{
    static std::string value = "nil";
    return value;
}

PrintFormatCsv::PrintFormatCsv(
        const PrintFormatCsvProperty& property,
        const dds::core::xtypes::DynamicType& type,
        std::ofstream& output_file) :
        property_(property),
        format_wrapper_(this),
        type_(type),
        output_file_(output_file),
        column_info_("", type_)
{

    initialize_native();
     // Add metadata column info    
    build_column_info(column_info_, type_);
    std::ostringstream string_stream;
    output_file_ << "timestamp";
    print_type_header(string_stream, column_info_);
    output_file << std::endl;
    
}

void PrintFormatCsv::initialize_native()
{
    NativePrintFormatCsv::get(format_wrapper_);
    format_wrapper_.enable_union_discriminator_printing = DDS_BOOLEAN_TRUE;
    format_wrapper_ .enable_top_level_type_printing = DDS_BOOLEAN_TRUE;
    format_wrapper_.enable_enum_as_string_printing = property_.enum_as_string()
            ?   DDS_BOOLEAN_TRUE
            :   DDS_BOOLEAN_FALSE;    
}

const PrintFormatCsvProperty& PrintFormatCsv::property() const
{
    return property_;
}


DDS_PrintFormat* PrintFormatCsv::native()
{
    return &format_wrapper_;
}

PrintFormatCsv& PrintFormatCsv::from_native(DDS_PrintFormat* native)
{
    PrintFormatWrapper<PrintFormatCsv> *wrapper =
            static_cast< PrintFormatWrapper<PrintFormatCsv>* >(native);
    return *wrapper->user_data_;
}

void PrintFormatCsv::start_data_conversion()
{
    cursor_stack_.clear();
    cursor_stack_.push_back(column_info_.first_child());
}

std::ofstream& PrintFormatCsv::output_file()
{
    return output_file_;
}

PrintFormatCsv::Cursor& PrintFormatCsv::cursor()
{
    return cursor_stack_.back();
}

void PrintFormatCsv::pop_cursor()
{
    cursor_stack_.pop_back();
}

void PrintFormatCsv::push_cursor()
{
    cursor_stack_.push_back(cursor()->first_child());
}

PrintFormatCsv::Cursor PrintFormatCsv::parent_cursor()
{
    /*
     * since the top Cursor in the stack may point to the end of children,
     * we always access the previous element, which is guaranteed to be
     * pointing a at valid info. From it, we retrieve the parent ColumnInfo.
     */
    PrintFormatCsv::CursorStack::const_reverse_iterator cursor_stack_it =
            cursor_stack_.crbegin();
    ++cursor_stack_it;
    return *cursor_stack_it;
}


void PrintFormatCsv::skip_cursor_siblings(
        RTIXMLSaveContext* save_context)
{
    // Skip as many columns as remaining elements in array/union
    PrintFormatCsv::CursorStack::const_reverse_iterator cursor_stack_it =
            cursor_stack_.crbegin();
    ++cursor_stack_it;
    PrintFormatCsv::Cursor parent_cursor = *cursor_stack_it;
    Cursor& cursor = this->cursor();
    if (cursor != parent_cursor->children().end()) {
        if (parent_cursor->type_kind() == TypeKind::ARRAY_TYPE
                || parent_cursor->type_kind() == TypeKind::SEQUENCE_TYPE
                || parent_cursor->type_kind() == TypeKind::UNION_TYPE) {
            for (;
                 cursor != parent_cursor->children().end();
                 ++cursor) {
                skip_cursor_columns(
                        cursor,
                        save_context,
                        property_.empty_member_value_representation());
            }
        }
    }
}

void PrintFormatCsv::skip_cursor(
        RTIXMLSaveContext* save_context)
{
    PrintFormatCsv::CursorStack::const_reverse_iterator cursor_stack_it =
            cursor_stack_.crbegin();
    ++cursor_stack_it;
    PrintFormatCsv::Cursor parent_cursor = *cursor_stack_it;
    Cursor& cursor = this->cursor();
    if (cursor != parent_cursor->children().end()) {
         skip_cursor_columns(
                 cursor,
                 save_context,
                 property_.empty_member_value_representation());
    }
}


void PrintFormatCsv::skip_cursor_columns(
        PrintFormatCsv::Cursor& cursor,
        RTIXMLSaveContext* save_context,
        const std::string& empty_member_value_rep)
{   
    // check for next sibling
    Cursor child_cursor = cursor->first_child();
    if (child_cursor == cursor->children().end()) {
        DDS_XMLHelper_save_freeform(
                save_context,
                "%s%s", 
                COLUMN_SEPARATOR_DEFAULT().c_str(),
                empty_member_value_rep.c_str());
    }

    // skip children
    for (; child_cursor != cursor->children().end(); ++child_cursor) {
        skip_cursor_columns(child_cursor, save_context, empty_member_value_rep);
    }
}

void PrintFormatCsv::skip_cursor(
        const std::string& member_name,
        struct RTIXMLSaveContext *save_context)
{
    Cursor& cursor = this->cursor();
    const ColumnInfo& parent_info = cursor->parent();

    for (; cursor != parent_info.children().end(); ++cursor) {
        if (cursor->name() == member_name) {
            return;
        } else {
            skip_cursor_columns(
                    cursor,
                    save_context,
                    property_.empty_member_value_representation());
        }
    }
}


void PrintFormatCsv::build_column_info(
        ColumnInfo& current_info,
        const dds::core::xtypes::DynamicType& member_type)
{
    switch (current_info.type_kind().underlying()) {

    case TypeKind::UNION_TYPE:
    {
        const UnionType& union_type =
                static_cast<const UnionType&> (member_type);               
        // Add discriminator column
        current_info.add_child(ColumnInfo(
               union_type.name() + ".disc",
               union_type.discriminator()));
        
        // recurse members 
        for (auto union_member : union_type.members()) {
            // complex member: branch tree
            ColumnInfo& child = current_info.add_child(ColumnInfo(
                    union_member.name(),
                    union_member.type()));
            build_column_info(
                    child,
                    union_member.type());
        }
    }
        break;

    case TypeKind::STRUCTURE_TYPE:
    {
        const StructType& struct_type =
                static_cast<const StructType&> (member_type);
        // Recurse members
        for (auto struct_member : struct_type.members()) {
            // complex member: branch tree
            ColumnInfo& child = current_info.add_child(ColumnInfo(
                    struct_member.name(),
                    struct_member.type()));
            build_column_info(
                    child,
                    struct_member.type());
        }
    }
        break;

    case TypeKind::ARRAY_TYPE:
    {
        const ArrayType& array_type =
                static_cast<const ArrayType &>(member_type);        
        std::vector<uint32_t> dimension_indexes;
        dimension_indexes.resize(array_type.dimension_count());
        uint32_t element_count = 0;
        while (element_count < array_type.total_element_count()) {
            std::ostringstream element_item;
            element_item << current_info.name();
            for (uint32_t j = 0; j < array_type.dimension_count(); j++) {
                element_item << "[" << dimension_indexes[j] << "]";
            }
            // add array item branch
            ColumnInfo& child = current_info.add_child(ColumnInfo(
                    element_item.str(),
                    array_type.content_type()));
            build_column_info(
                    child,
                    array_type.content_type());

            ++dimension_indexes[array_type.dimension_count() - 1];
            for (uint32_t j = array_type.dimension_count() - 1; j > 0; j--) {
                if (dimension_indexes[j] ==  array_type.dimension(j)) {
                    ++dimension_indexes[j - 1];
                    dimension_indexes[j] = 0;
                }
            }

            ++element_count;
        }
    }                
        break;

    case TypeKind::SEQUENCE_TYPE:
    {
        const SequenceType& sequence_type =
                static_cast<const SequenceType &> (member_type);
        for (uint32_t i = 0; i < sequence_type.bounds(); i++) {
            std::ostringstream element_item;
            element_item << current_info.name() << "[" << i << "]";

            // add array item branch
            ColumnInfo& child = current_info.add_child(ColumnInfo(
                    element_item.str(),
                    sequence_type.content_type()));
            build_column_info(
                    child,
                    sequence_type.content_type());
        }
    }
        break;

    case TypeKind::ALIAS_TYPE:
    {
        const AliasType& alias_type =
                static_cast<const AliasType &>(member_type);
        current_info.type_kind(alias_type.related_type().kind());
        build_column_info(
                current_info,
                alias_type.related_type());
    }
        
        break;
        
    default:
        // leaf reached
        break;
    }
}

void PrintFormatCsv::print_type_header(
        std::ostringstream& string_stream,
        const ColumnInfo& current_info)
{
    for (auto& child : current_info.children()) {
        std::ostringstream child_stream;
        
        if (!current_info.has_parent()) {
            child_stream << ",";
        }

        child_stream << string_stream.str();
        if (!child.is_collection()) {            
            child_stream << "." ;
            child_stream << child.name();
        }
      
        PrintFormatCsv::print_type_header(child_stream, child);
    }
    
    if (current_info.children().size() == 0) {
        output_file() << string_stream.str();
    }
}


/*
 * --- ColumnInfo -------------------------------------------------------------
 */

PrintFormatCsv::ColumnInfo::ColumnInfo() :
    parent_(NULL),
    type_kind_(TypeKind::NO_TYPE)
{
}

PrintFormatCsv::ColumnInfo::ColumnInfo(const ColumnInfo& other) :
    parent_(other.parent_),
    name_(other.name_),
    type_kind_(other.type_kind_),
    children_(other.children_)
{
}


PrintFormatCsv::ColumnInfo::ColumnInfo(
        const std::string& name,
        const dds::core::xtypes::DynamicType& type) :
    parent_(NULL),
    name_(name),
    type_kind_(type.kind())
{
}


const PrintFormatCsv::ColumnInfo&
PrintFormatCsv::ColumnInfo::parent() const
{
    return *parent_;
}


PrintFormatCsv::ColumnInfo&
PrintFormatCsv::ColumnInfo::add_child(const ColumnInfo& info)
{
    children_.push_back(info);
    children_.back().parent_ = this;
    
    return children_.back();
}


PrintFormatCsv::ColumnInfo::iterator
PrintFormatCsv::ColumnInfo::first_child() const
{
    return children_.begin();
}

const TypeKind& PrintFormatCsv::ColumnInfo::type_kind() const
{
    return type_kind_;
}

PrintFormatCsv::ColumnInfo& PrintFormatCsv::ColumnInfo::type_kind(
        const dds::core::xtypes::TypeKind& type_kind)
{
    type_kind_ = type_kind;

    return *this;
}


const std::vector<PrintFormatCsv::ColumnInfo>&
PrintFormatCsv::ColumnInfo::children() const
{
    return children_;
}

const std::string& PrintFormatCsv::ColumnInfo::name() const
{
    return name_;
}

bool PrintFormatCsv::ColumnInfo::has_parent() const
{
    return (parent_ != NULL);
}

bool PrintFormatCsv::ColumnInfo::is_collection() const
{
    return (type_kind() == TypeKind::ARRAY_TYPE
                || type_kind() == TypeKind::SEQUENCE_TYPE);
}


std::ostream& operator<<(
        std::ostream& os,
        const PrintFormatCsv::ColumnInfo& info)
{
    os << "name: " << info.name_
            << ", parent: " << info.parent_
            << ", children: " << info.children_.size();

    return os;
}

} } }
