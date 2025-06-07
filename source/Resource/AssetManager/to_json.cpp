//
// Created by Administrator on 25-5-7.
//

#include "to_json.h"
#include <iostream>

#include "Function/Framework/Level/Level.h"
#include "Function/Framework/Object/PrimaryObject.h"
#include "Function/Message/Message.h"
#include "Resource/AssetManager/AssetManager.h"

namespace TinyRenderer {
    namespace io {
        void to_json_recursively(const rttr::instance &obj, ordered_json &json);
        bool write_variant(const std::string &name, rttr::variant &var, ordered_json &json);

        // 处理基本类型 包括运算类型 字符串 GUID PrimaryObject* Resource*
        bool write_atomic_types_to_json(const std::string &name, const rttr::type &t, const rttr::variant &var, ordered_json &json) {
            // std::string t_str = t.get_name().to_string();
            // 处理算术类型
            if (t.is_arithmetic()) {
                if (t == rttr::type::get<bool>())
                    json[name] = var.to_bool();
                else if (t == rttr::type::get<char>())
                    json[name] = var.to_string();
                else if (t == rttr::type::get<int8_t>())
                    json[name] = var.to_int8();
                else if (t == rttr::type::get<int16_t>())
                    json[name] = var.to_int16();
                else if (t == rttr::type::get<int32_t>())
                    json[name] = var.to_int32();
                else if (t == rttr::type::get<int64_t>())
                    json[name] = var.to_int64();
                else if (t == rttr::type::get<uint8_t>())
                    json[name] = var.to_uint8();
                else if (t == rttr::type::get<uint16_t>())
                    json[name] = var.to_uint16();
                else if (t == rttr::type::get<uint32_t>())
                    json[name] = var.to_uint32();
                else if (t == rttr::type::get<uint64_t>())
                    json[name] = var.to_uint64();
                else if (t == rttr::type::get<float>())
                    json[name] = var.to_double();// float 转为 double 以兼容 JSON 数值类型
                else if (t == rttr::type::get<double>())
                    json[name] = var.to_double();
                return true;
            }
            // 处理枚举类型
            else if (t.is_enumeration()) {
                bool ok = false;
                auto result = var.to_string(&ok);
                // 尝试获取枚举字面量
                if (ok) {
                    json[name] = var.to_string();
                }
                // 尝试转为int类型
                else {
                    ok = false;
                    auto value = var.to_uint64(&ok);
                    if (ok)
                        json[name] = value;
                    else
                        json[name] = nullptr;
                }

                return true;
            }
            // 处理字符串类型
            else if (t == rttr::type::get<std::string>()) {
                json[name] = var.to_string();
                return true;
            }
            // 把GUID当做原子类型直接处理
            else if (t == rttr::type::get<GUID>()) {
                GUID guid = var.get_value<GUID>();
                json[name] = guid.to_string();
                return true;
            }
            // PrimaryObject*类型直接保存为GUID，并且将它保存到另一个GUID.json文件。Resource类型也是走这条路线
            else if((t.is_derived_from<HierarchyNode>() || t.is_derived_from<PrimaryObject>()) && t.is_pointer()) {
                rttr::variant converted_value = var.convert<PrimaryObject*>();
                if (converted_value.is_valid()) {
                    PrimaryObject* obj_ptr = converted_value.get_value<PrimaryObject*>();
                    if (obj_ptr != nullptr) {
                        std::string guid = obj_ptr->get_guid().to_string();
                        json[name] = guid;
                    }
                }
                return true;
            }
            // Object*的对象会走正常对象处理的路线
            return false;
        }

        static void write_array(const std::string &container_name, const rttr::variant_sequential_view &view, ordered_json &json) {
            ordered_json array_json;
            for (const auto &item: view) {
                // 处理容器嵌套情况
                if (item.is_sequential_container()) {
                    ordered_json sequential_container_json;
                    write_array(container_name, item.create_sequential_view(), sequential_container_json);
                    array_json.push_back(sequential_container_json[container_name]);
                }
                else {
                    // 去除包裹类型，获取实际的值(去除atomic const)
                    rttr::variant wrapped_var = item.extract_wrapped_value();
                    rttr::type value_type = wrapped_var.get_type();

                    ordered_json value_json;
                    if (write_atomic_types_to_json(container_name, value_type, wrapped_var, value_json)){
                        array_json.push_back(value_json[container_name]);
                    }
                    // 处理复合类型
                    else {
                        ordered_json obj_json;
                        to_json_recursively(wrapped_var, obj_json);
                        array_json.push_back(obj_json);
                    }
                }
            }
            json[container_name] = std::move(array_json);
        }

        void write_associative_container(const std::string &container_name,
                                         const rttr::variant_associative_view &view,
                                         ordered_json &json) {
            static const rttr::string_view key_name("key");
            static const rttr::string_view value_name("value");

            ordered_json container_json;

            if (view.is_key_only_type())// 处理set类型
            {
                for (auto &item: view) {
                    ordered_json element;
                    rttr::variant item_first(item.first);
                    write_variant(container_name, item_first, element);
                    container_json.push_back(element);
                }
            } else// 处理map类型
            {
                for (auto &item: view) {
                    ordered_json kv_pair;
                    rttr::variant item_first(item.first);
                    rttr::variant item_second(item.second);

                    write_variant(key_name.data(), item_first, kv_pair);
                    write_variant(value_name.data(), item_second, kv_pair);
                    container_json[container_name].push_back(kv_pair);// 将键值对对象加入数组
                }
            }
            json[container_name] = container_json[container_name];// 将数组赋值给容器字段
        }

        bool write_variant(const std::string &name, rttr::variant &var, ordered_json &json) {
            auto value_type = var.get_type();
            // 不会去除指针修饰符
            auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;

            bool is_wrapper = wrapped_type != value_type;

            // 处理基本类型变量,和基本指针类型变量,基本包裹类型变量
            if (write_atomic_types_to_json(name, is_wrapper ? wrapped_type : value_type,
                                           is_wrapper ? var.extract_wrapped_value() : var, json)) {
                                           }
            // 处理有序容器
            else if (var.is_sequential_container()) {
                write_array(name, var.create_sequential_view(), json);
            }
            // 处理关联容器
            else if (var.is_associative_container()) {
                write_associative_container(name, var.create_associative_view(), json);
            }
            // 处理复合类型
            else {
                // 获取所有子属性
                auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
                // 如果获取到了所有子属性
                if (!child_props.empty()) {
                    ordered_json obj_json;
                    to_json_recursively(var, obj_json);
                    json[name] = obj_json;

                } else {
                    bool ok = false;
                    auto text = var.to_string(&ok);
                    // 如果不成功则返回false
                    if (!ok) {
                        json[name] = text;
                        return false;
                    }
                    json[name] = text;
                }
            }
            return true;
        }

        void to_json_recursively(const rttr::instance &obj, ordered_json &json) {
            // 判断原始类型是否为包裹类型，是则解压得到原始类型，否则直接使用这个类型
            rttr::instance raw_obj = obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj;
            // 获取运行时对应的类型属性
            auto prop_list = raw_obj.get_derived_type().get_properties();
            // 遍历所有属性，按照name : value对形式保存
            for (auto &prop: prop_list) {
                // 判断是否需要进行序列化
                if (!prop.get_metadata(PROPERTY_FLAG_SERIALIZE).to_bool())
                    continue;

                rttr::variant prop_value = prop.get_value(obj);
                std::string prop_name = prop.get_name().to_string();
                if (!prop_value)
                    continue;

                if (!write_variant(prop_name, prop_value, json)) {
                    std::cerr << "cannot serialize property: " << prop_name << std::endl;
                }
            }
        }

        ordered_json to_json(rttr::instance obj) {
            if (!obj.is_valid())
                return json();
            ordered_json res_json;
            // res_json["__type__"] = obj.get_derived_type().get_name().to_string();
            to_json_recursively(obj, res_json);

            return res_json;
        }
    }
}