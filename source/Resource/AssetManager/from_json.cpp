//
// Created by Administrator on 25-5-9.
//

#include "from_json.h"

#include "Function/Components/login/login.h"
#include "Resource/AssetManager/AssetManager.h"
#include "Function/Framework/GameObject/GameObject.h"
#include "Resource/GUID/GUID.h"
#include "Function/Framework/Object/Object.h"

using namespace std;
using namespace rttr;

namespace TinyRenderer {
    namespace io {
        variant extract_basic_type(const json& j, const rttr::type& t, bool is_guidToObject);
        void from_json_recursively(instance obj2, json& source_json);
        void write_associative_view_recursively(variant_associative_view &view, json &json_array_value, bool is_guidToObject);

        void write_array_recursively(variant_sequential_view &view, json &json_array_value, bool is_guidToObject)
        {
            view.set_size(json_array_value.size());
            // 获取数组内的元素类型
            const type array_type = view.get_rank_type(1);

            for(size_t i = 0; i < json_array_value.size(); i++)
            {
                auto& json_index_value = json_array_value[i];
                if (json_index_value.empty()) {
                    // TODO:测试值为null时是不是走这个路，是则考虑删除json_array_value数组中这一节点
                    continue;
                }
                // 处理嵌套容器情况
                if(json_index_value.is_array())
                {
                    auto sub_array_view = view.get_value(i).create_sequential_view();
                    write_array_recursively(sub_array_view, json_index_value, is_guidToObject);
                }
                // property为Object*类型，通过查找GUID获得真正的值
                else if (array_type.is_derived_from(rttr::type::get<PrimaryObject>()) && array_type.is_pointer()) {
                    // TODO:判断是Object*还是Resource*，Object*会走上一条分支，Resource*可以走下一个分支
                    // 查看序列化标志是否要反序列化
                    if (!is_guidToObject)
                        return;

                    // 获取GUID
                    GUID guid(GUID(json_index_value.get<string>()));
                    if (!guid.is_valid())
                        continue;

                    // 反序列化出得到对应GUID文件的数据
                    variant var = AssetManager::get_instance().load_variant(guid);
                    if (var.convert(view.get_value_type())) {
                        view.set_value(i, var);
                    }
                }
                // 处理对象情况 遇到Object*也会走这个分支
                else if(json_index_value.is_object())
                {
                    // std::cout << json_index_value.dump(4) << std::endl;
                    variant var_tmp = view.get_value(i);
                    // TODO:这里需要为var_tmp判断是否为Object*类型，是则获取其中的__type__
                    if (array_type.is_derived_from(rttr::type::get<Object>()) && array_type.is_pointer()) {
                        if (!json_index_value.contains("__type__")) {
                            LOG_ERROR("__type__ not found at type : " << array_type.get_name().to_string());
                        }
                        string type = json_index_value["__type__"];
                        var_tmp = rttr::type::get_by_name(type).create();
                    }

                    if (!var_tmp.is_valid())
                        continue;

                    // variant wrapped_var = var_tmp.extract_wrapped_value();
                    from_json_recursively(var_tmp, json_index_value);

                    if (var_tmp.convert(array_type))
                        view.set_value(i, var_tmp);
                }
                // 处理基本类型情况
                else {
                    variant extracted_value = extract_basic_type(json_index_value, array_type, is_guidToObject);
                    if(extracted_value.convert(array_type))
                        view.set_value(i, extracted_value);
                }
            }
        }

        variant extract_value(json::iterator &itr, const type& t, bool is_guidToObject) {
            auto& json_value = itr.value();
            // std::cout << json_value.dump(4) << std::endl;
            variant extracted_value = extract_basic_type(json_value, t, is_guidToObject);
            const bool could_convert = extracted_value.convert(t);
            if(!could_convert) {
                if(json_value.is_object()) {
                    constructor ctor = t.get_constructor();
                    for(auto& item : t.get_constructors()) {
                        if(item.get_instantiated_type() == t)
                            ctor = item;
                    }
                    extracted_value = ctor.invoke();
                    from_json_recursively(extracted_value, json_value);
                }
                else if (json_value.is_array()) {
                    if (t.is_sequential_container()) {
                        // 设置extract_value
                        // std::cout << "sequential container" << std::endl;
                        // std::string t_str = t.get_name().to_string();

                        // 创建一个数组类型的variant。如vector<Object*>
                        variant var_array = t.create();
                        // auto var_array_type = var_array.get_type().get_name().to_string();
                        if (var_array.is_valid()) {
                            auto seq_view = var_array.create_sequential_view();
                            write_array_recursively(seq_view, json_value, is_guidToObject);
                            extracted_value = var_array;
                        }
                    }
                    else if (t.is_associative_container()) {
                        // std::cout << "associative container" << std::endl;
                        variant var_array = t.create();
                        if (var_array.is_valid()) {
                            auto associate_view = var_array.create_associative_view();
                            write_associative_view_recursively(associate_view, json_value, is_guidToObject);
                            extracted_value = var_array;
                        }
                    }
                }
            } // TODO:判断是否为数组类型
            return extracted_value;
        }

        // associative_view支持对象类型 vector<Component*> list<Component*>类型
        void write_associative_view_recursively(variant_associative_view &view, json &json_array_value, bool is_guidToObject)
        {
            const type key_type = view.get_key_type();
            const type value_type = view.get_value_type();

            for(size_t i = 0; i < json_array_value.size(); i++)
            {
                auto& json_index_value = json_array_value[i];
                // 处理map之类的数据结构
                if(json_index_value.is_object()) {
                    auto key_itr = json_index_value.find("key");
                    auto value_itr = json_index_value.find("value");

                    if(key_itr != json_index_value.end() && value_itr != json_index_value.end())
                    {
                        // 显式处理 GUID 键或值
                        variant key_var = extract_value(key_itr, key_type, is_guidToObject);

                        variant value_var = extract_value(value_itr, value_type, is_guidToObject);

                        auto value_var_type_str = value_var.get_type().get_name().to_string();
                        auto value_type_str = value_type.get_name().to_string();

                        if(key_var.convert(key_type) && value_var.convert(value_type))
                        {
                            view.insert(key_var, value_var);
                        }
                    }
                }
                // 处理set之类的数据结构
                else
                {
                    variant extracted_value = extract_basic_type(json_index_value, key_type, is_guidToObject);

                    if(extracted_value && extracted_value.convert(view.get_key_type()))
                        view.insert(extracted_value);
                }
            }
        }

        // 获取基本类型
        variant extract_basic_type(const json& j, const rttr::type& t, bool is_guidToObject)
        {
            // 布尔类型
            if (j.is_boolean())
                return j.get<bool>();

            // 数值类型细分（整型、浮点型）
            else if (j.is_number())
            {
                if (j.is_number_integer())
                {
                    // 处理带符号整数
                    if (!j.is_number_unsigned())
                    {
                        const int64_t value = j.get<int64_t>();
                        // 根据值范围选择最合适的整数类型
                        if (value >= std::numeric_limits<int8_t>::min() && value <= std::numeric_limits<int8_t>::max())
                            return static_cast<int8_t>(value);
                        else if (value >= std::numeric_limits<int16_t>::min() && value <= std::numeric_limits<int16_t>::max())
                            return static_cast<int16_t>(value);
                        else if (value >= std::numeric_limits<int32_t>::min() && value <= std::numeric_limits<int32_t>::max())
                            return static_cast<int32_t>(value);
                        else
                            return static_cast<int64_t>(value);
                    }
                    else // 无符号整数
                    {
                        const uint64_t value = j.get<uint64_t>();
                        if (value <= std::numeric_limits<uint8_t>::max())
                            return static_cast<uint8_t>(value);
                        else if (value <= std::numeric_limits<uint16_t>::max())
                            return static_cast<uint16_t>(value);
                        else if (value <= std::numeric_limits<uint32_t>::max())
                            return static_cast<uint32_t>(value);
                        else
                            return static_cast<uint64_t>(value);
                    }
                }
                else if (j.is_number_float()) // 浮点型
                {
                    // 优先转为 double，兼容 JSON 数值精度
                    return j.get<double>();
                }
            }

            // 字符类型（JSON 无 char 类型，需从单字符字符串转换）
            else if (j.is_string() && j.get<std::string>().size() == 1)
                return j.get<std::string>()[0];

            // 处理GUID类型
            else if (t == rttr::type::get<GUID>()) {
                return GUID(j.get<std::string>());
            }

            // 字符串类型
            else if (j.is_string())
                return j.get<std::string>();

            // 枚举类型（假设 JSON 存储为字符串或数值）
            else if (j.is_string() || j.is_number())
            {
                return j.is_string() ? variant(j.get<std::string>()) : variant(j.get<int64_t>());
            }

            // TODO:要实现一个GUID->Reource*的分支 暂时使用Object*
            else if (t.is_derived_from(rttr::type::get<PrimaryObject>()) && t.is_pointer()) {
                if (is_guidToObject) {
                    GUID guid(GUID(j.get<string>()));
                    if (guid.is_valid()) {
                        variant var = AssetManager::get_instance().load_variant(guid);
                        return var;
                    }
                }
            }

            return variant();
        }

        void from_json_recursively(instance obj2, json& source_json)
        {
            // 获取obj2的原始类型，并且判断是否是包装类型。是则获取其原始类型
            instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
            const auto prop_list = obj.get_derived_type().get_properties();

            // 遍历obj包含的所有属性
            for(auto& prop : prop_list)
            {
                string prop_name = prop.get_name().to_string();
                if (!source_json.contains(prop_name))
                    continue;

                // 根据属性名称在json中获取对应的值
                auto& json_value = source_json[prop_name];
                const type prop_type = prop.get_type();
                string type_string = prop_type.get_name().to_string();

                // 这个属性是否需要从GUID生成Object*实例
                bool is_guidToObject = prop.get_metadata(PROPERTY_FLAG_GUIDTOOBJECT).to_bool();
                // 处理数组类型
                if(json_value.is_array()) {
                    variant var = prop.get_value(obj);
                    if (prop_type.is_sequential_container()) {
                        auto view = var.create_sequential_view();
                        write_array_recursively(view, json_value, is_guidToObject);
                    }
                    else if (prop_type.is_associative_container()) {
                        auto associative_view = var.create_associative_view();
                        write_associative_view_recursively(associative_view, json_value, is_guidToObject);
                    }
                    if (var.convert(prop.get_type()))
                        prop.set_value(obj, var);
                }
                // 特殊处理GUID
                else if(prop_type == rttr::type::get<GUID>()){
                    string guid_str = json_value.get<string>();
                    GUID guid(guid_str);
                    if (guid.is_valid())
                        prop.set_value(obj, guid);
                }
                // 处理PrimaryObject*类型 直接反序列化对应的GUID。如Level GO Resource都是走这条路线。Resource通过重载set_guid自动实现加载
                else if ((prop_type.is_derived_from(rttr::type::get<PrimaryObject>()) && prop_type.is_pointer()) || prop_type == rttr::type::get<HierarchyNode*>()) {
                    if (!is_guidToObject)
                        continue;
                    GUID guid(json_value.get<string>());
                    variant var = AssetManager::get_instance().load_variant(guid);
                    if (var.convert(prop_type))
                        prop.set_value(obj, var);
                }
                // 处理对象类型。Component和一般的复合成员对象都是走这条路线
                else if(json_value.is_object()) {
                    variant var = prop.get_value(obj);

                    // 判断Object*类型，因为Object*需要调用t.create提前开辟空间
                    if (prop_type.is_derived_from(rttr::type::get<Object>()) && prop_type.is_pointer()) {
                        var = prop_type.create();
                        // 创建的Object不合法
                        if (!var.is_valid()) {
                            LOG_ERROR("Failed to create object from json" << " property type:" << prop_type.get_name().to_string() << "  json: " << json_value.dump(4));
                            continue;
                        }
                    }
                    from_json_recursively(var, json_value);
                    if (var.convert(prop_type))
                        prop.set_value(obj, var);
                }
                // 处理基础类型
                else{
                    variant extracted_value = extract_basic_type(json_value, prop_type, is_guidToObject);
                    if(extracted_value.convert(prop_type))
                        prop.set_value(obj, extracted_value);
                }
            }
        }

        void from_json(const string& string_json, instance obj)
        {
            json serialized_json = json::parse(string_json);
            // 反序列化数据到对象
            from_json_recursively(obj, serialized_json);
        }

        void from_json(json& resource_json, instance obj)
        {
            from_json_recursively(obj, resource_json);
        }
    }// namespace io
}