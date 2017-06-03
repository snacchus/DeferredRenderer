#ifndef SCRIPTING_UTILITY_HPP
#define SCRIPTING_UTILITY_HPP

#include <string>
#include <type_traits>

#include "lua.hpp"
#include "boost/polymorphic_pointer_cast.hpp"

class Object;

namespace scripting
{

	inline int pseudo_index(lua_State* L, int i)
	{
		return lua_gettop(L) + i + 1;
	}

	inline void set_field(lua_State* L, int idx, const std::string& k)
	{
		lua_setfield(L, idx, k.c_str());
	}

	inline void set_field(lua_State* L, int idx, const char* k)
	{
		lua_setfield(L, idx, k);
	}

	void raise_error(lua_State* L, const std::string& msg);

	void stack_dump(lua_State* L);
	std::string value_to_string(lua_State* L, int idx);
	inline std::string value_typename(lua_State* L, int idx)
	{
		return lua_typename(L, lua_type(L, idx));
	}

	void swap_top_2(lua_State* L);

	Object* get_object(lua_State* L, int idx);
	void push_object(lua_State* L, Object* obj);
	Object* check_object_arg(lua_State* L, int arg);

	template<typename T>
	struct assert_false : std::false_type { };

	template<typename T>
	struct type_handler
	{
		static_assert(assert_false<T>::value, "This type is not supported by the scripting environment!");
	};

	template<typename T>
	T get_value(lua_State* L, int idx = -1)
	{
		return type_handler<T>::get_value(L, idx);
	}

	template<typename T>
	void push_value(lua_State* L, T&& value)
	{
		type_handler<std::decay_t<T>>::push_value(L, std::forward<T>(value));
	}

	template<typename T>
	T check_arg(lua_State* L, int arg)
	{
		return type_handler<T>::check_arg(L, arg);
	}

	template<typename T>
	T* check_self(lua_State* L, int arg = 1)
	{
		static_assert(is_object<T>::value, "self should be converted to an object type!");
		return check_arg<T*>(L, arg);
	}

	inline void push_values(lua_State* L) { }

	template<typename T, typename... Args>
	void push_values(lua_State* L, T&& value, Args&&... args)
	{
		push_value(L, std::forward<T>(value));
		push_values(L, std::forward<Args>(args)...);
	}

	template<typename ObjT, typename ValueT>
	using obj_getter_func = ValueT(ObjT::*)();

	template<typename ObjT, typename ValueT>
	using const_obj_getter_func = ValueT(ObjT::*)() const;

	template<typename ObjT, typename ValueT>
	using obj_setter_func = void (ObjT::*)(ValueT);

	template<typename ObjT, typename ValueT>
	using obj_member_ptr = ValueT ObjT::*;

	template<typename T, typename U>
	void push_member(lua_State* L, T* self, obj_getter_func<T, U> member)
	{
		push_value<U>(L, (self->*member)());
	}

	template<typename T, typename U>
	void push_member(lua_State* L, const T* self, const_obj_getter_func<T, U> member)
	{
		push_value<U>(L, (self->*member)());
	}

	template<typename T, typename U>
	auto push_member(lua_State* L, const T* self, obj_member_ptr<T, U> member) -> std::enable_if_t<!std::is_function<U>::value>
	{
		push_value<U>(L, self->*member);
	}

	template<typename T, typename U>
	void check_member(lua_State* L, int arg, T* self, obj_setter_func<T, U> member)
	{
		(self->*member)(check_arg<std::decay_t<U>>(L, arg));
	}

	template<typename T, typename U>
	void check_member(lua_State* L, int arg, T* self, obj_member_ptr<T, U> member)
	{
		self->*member = check_arg<U>(L, arg);
	}

	template<typename T, typename U>
	int getter_helper(lua_State* L, obj_getter_func<T, U> getMember)
	{
		push_member(L, check_self<T>(L), getMember);
		return 1;
	}

	template<typename T, typename U>
	auto getter_helper(lua_State* L, const_obj_getter_func<T, U> getMember) -> std::enable_if_t<!std::is_pointer<U>::value, int>
	{
		push_member(L, check_self<T>(L), getMember);
		return 1;
	}

	template<typename T, typename U>
	int getter_helper(lua_State* L, obj_member_ptr<T, U> getMember)
	{
		push_member(L, check_self<T>(L), getMember);
		return 1;
	}

	template<typename T, typename U>
	int setter_helper(lua_State* L, obj_setter_func<T, U> setMember)
	{
		check_member(L, 2, check_self<T>(L), setMember);
		return 0;
	}

	template<typename T, typename U>
	int setter_helper(lua_State* L, obj_member_ptr<T, U> setMember)
	{
		check_member(L, 2, check_self<T>(L), setMember);
		return 0;
	}

	template<>
	struct type_handler<bool>
	{
		static bool get_value(lua_State* L, int idx)
		{
			return lua_toboolean(L, idx);
		}

		static void push_value(lua_State* L, bool value)
		{
			lua_pushboolean(L, value);
		}

		static bool check_arg(lua_State* L, int arg)
		{
			luaL_checkany(L, arg);
			return lua_toboolean(L, arg);
		}
	};

	template<>
	struct type_handler<lua_Unsigned>
	{
		static lua_Unsigned get_value(lua_State* L, int idx)
		{
			return lua_tounsigned(L, idx);
		}

		static void push_value(lua_State* L, lua_Unsigned value)
		{
			lua_pushunsigned(L, value);
		}

		static lua_Unsigned check_arg(lua_State* L, int arg)
		{
			return luaL_checkunsigned(L, arg);
		}
	};

	template<>
	struct type_handler<unsigned int> : public type_handler<lua_Unsigned> { };

	template<>
	struct type_handler<unsigned short> : public type_handler<lua_Unsigned> { };

	template<>
	struct type_handler<unsigned char> : public type_handler<lua_Unsigned> { };

	template<>
	struct type_handler<lua_Integer>
	{
		static lua_Integer get_value(lua_State* L, int idx)
		{
			return lua_tointeger(L, idx);
		}

		static void push_value(lua_State* L, lua_Integer value)
		{
			lua_pushinteger(L, value);
		}

		static lua_Integer check_arg(lua_State* L, int arg)
		{
			return luaL_checkinteger(L, arg);
		}
	};

	template<>
	struct type_handler<int> : public type_handler<lua_Integer> { };

	template<>
	struct type_handler<short> : public type_handler<lua_Integer> { };

	template<>
	struct type_handler<char> : public type_handler<lua_Integer> { };

	template<>
	struct type_handler<lua_Number>
	{
		static lua_Number get_value(lua_State* L, int idx)
		{
			return lua_tonumber(L, idx);
		}

		static void push_value(lua_State* L, lua_Number value)
		{
			lua_pushnumber(L, value);
		}

		static lua_Number check_arg(lua_State* L, int arg)
		{
			return luaL_checknumber(L, arg);
		}
	};

	template<>
	struct type_handler<float> : public type_handler<lua_Number> { };

	template<>
	struct type_handler<char*>
	{
		static const char* get_value(lua_State* L, int idx)
		{
			return lua_tostring(L, idx);
		}

		static void push_value(lua_State* L, const char* value)
		{
			lua_pushstring(L, value);
		}

		static const char* check_arg(lua_State* L, int arg)
		{
			return luaL_checkstring(L, arg);
		}
	};

	template<>
	struct type_handler<std::string>
	{
		static std::string get_value(lua_State* L, int idx)
		{
			return lua_tostring(L, idx);
		}

		static void push_value(lua_State* L, const std::string& value)
		{
			lua_pushstring(L, value.c_str());
		}

		static std::string check_arg(lua_State* L, int arg)
		{
			return luaL_checkstring(L, arg);
		}
	};

	template<typename T>
	using is_object = std::is_base_of<Object, T>;

	template<typename T, typename Enable = void>
	struct ptr_type_handler
	{
		static T* get_value(lua_State* L, int idx)
		{
			return static_cast<T*>(lua_touserdata(L, idx));
		}

		static void push_value(lua_State* L, T* value)
		{
			lua_pushlightuserdata(L, value);
		}

		static T* check_arg(lua_State* L, int arg)
		{
			luaL_checktype(L, arg, LUA_TLIGHTUSERDATA);
			return ptr_type_handler::get_value(L, arg);
		}
	};

	template<typename T>
	struct ptr_type_handler<T, std::enable_if_t<std::is_object<T>::value>>
	{
		static T* get_value(lua_State* L, int idx)
		{
			// NOTE: polymorphic_pointer_downcast only performs runtime checks in debug configs. if this causes too many problems, change this to dynamic_cast
			return boost::polymorphic_pointer_downcast<T>(get_object(L, idx));
		}

		static void push_value(lua_State* L, T* value)
		{
			push_object(L, value);
		}

		static T* check_arg(lua_State* L, int arg)
		{
			// NOTE: polymorphic_pointer_downcast only performs runtime checks in debug configs. if this causes too many problems, change this to dynamic_cast
			T* obj = boost::polymorphic_pointer_downcast<T>(check_object_arg(L, arg));
			if (!obj) {
				luaL_argerror(L, arg, "invalid object pointer");
			}
			return obj;
		}
	};

	template<typename T>
	struct type_handler<T*> : public ptr_type_handler<T>
	{ };
}

#endif // SCRIPTING_UTILITY_HPP
