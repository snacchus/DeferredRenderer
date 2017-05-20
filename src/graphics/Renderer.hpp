#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "core/Component.hpp"
#include "Drawable.hpp"
#include "util/json_interpreter.hpp"
#include "util/bounds.hpp"

class Material;

class Renderer : public Component
{
public:
	explicit Renderer(Entity* parent);

	std::size_t materialCount() const { return m_materials.size(); }

	Material* getMaterial(unsigned int index) { return m_materials[index]; }
	const Material* getMaterial(unsigned int index) const { return m_materials[index]; }

	void addMaterial(Material* mat) { m_materials.push_back(mat); }
	void clearMaterials() { m_materials.clear(); }

	const Drawable* getDrawable(unsigned int index) const { return getDrawable_impl(index); }

	bool isVisible() const { return hasGeometry(); }

	COMPONENT_DISALLOW_MULTIPLE;

protected:
	virtual const Drawable* getDrawable_impl(unsigned int index) const = 0;
	virtual bool hasGeometry() const = 0;
	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json) override;

private:
	std::vector<Material*> m_materials;

	static json_interpreter<Renderer> s_properties;

	void extractMaterials(const nlohmann::json& json);
};

#endif // RENDERER_HPP