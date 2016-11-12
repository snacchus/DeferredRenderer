#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "util/import.hpp"
#include "util/type_registry.hpp"
#include "util/json_initializable.hpp"
#include "graphics/shader_property.hpp"

class Effect;

class Material : public json_initializable<Material>
{
public:
	Material();

	Effect* getEffect() { return m_effect; }
	const Effect* getEffect() const { return m_effect; }
	void setEffect(Effect* effect);

	const shader_property* getProperty(const std::string& name) const;

	template<typename T>
	bool getPropertyValue(const std::string& name, T& value) const
	{
		const shader_property* prop = getProperty(name);
		if (prop) {
			value = prop->value;
			return true;
		}
		return false;
	}

	template<typename T>
	T getPropertyValue(const std::string& name) const
	{
		T tmp;
		getProperty(name, tmp);
		return std::move(tmp);
	}

	template<typename T>
	void setPropertyValue(const std::string& name, const T& newValue)
	{
		auto it = m_properties.find(name);
		if (it != m_properties.end()) {
			m_properties.modify(it, [&json](shader_property& prop) {
				prop.set(newValue);
			});
		}
	}

private:
	Effect* m_effect;
	shader_property_map m_properties;

	void apply_json_impl(const nlohmann::json& json);

	void setPropFromJson(const std::string& name, const nlohmann::json& json);

	friend struct json_initializable<Material>;

	REGISTER_OBJECT_TYPE_DECL(Material);
};

#endif // MATERIAL_HPP