#ifndef __INSTANCE_H__
#define __INSTANCE_H__

#include "Validation.h"

#include <assert.h>
#include <string.h>
#include <string>
#include <vector>

class PickList
{
protected:
	std::vector<uint32_t> pick_list_indices;

	std::vector<std::string> pick_list_names;

	std::vector<const char*> pick_list_names_ptrs;

public:
	PickList();

	virtual ~PickList();

protected:
	void RefreshPickList();

public:
	virtual const char* Name(uint32_t inx) const = 0;
	
	virtual uint32_t Count() const = 0;

	virtual std::string PickListName() const = 0;

public:
	bool Pick(std::initializer_list<const char*> list);

	bool Pick(const char* name);

	bool Pick(const uint32_t inx);

	void UnPick(const char* name);
	
	void PickAll();
	
	void Clear();
	
	int IndexOf(const char* name) const;

	bool IsPicked(const char* name) const;

	const char* const* GetPickList() const;

	uint32_t PickCount() const;

	void Print() const;
};
//----------------------------------------------------------------

//----------------------------Layers-----------------------------
class Layers : public PickList
{
private:
	std::vector<VkLayerProperties> item_list;

public:
	Layers();
	
public:
	const char* Name(uint32_t inx) const override;

	uint32_t Count() const override;

	std::string PickListName() const override;
};
//----------------------------------------------------------------

//--------------------------Extensions---------------------------
class Extensions : public PickList
{
private:
	std::vector<VkExtensionProperties> item_list;

public:
	Extensions(const char* layerName = nullptr);

public:
	const char* Name(uint32_t inx) const override;

	uint32_t Count() const override;

	std::string PickListName() const override;
};
//----------------------------------------------------------------

//----------------------Device Extensions-------------------------
class DeviceExtensions : public PickList
{
private:
	std::vector<VkExtensionProperties> item_list;

public:
	DeviceExtensions();

	DeviceExtensions& operator=(const DeviceExtensions& other);

	DeviceExtensions(const DeviceExtensions& other);

public:
	void Init(VkPhysicalDevice phy, const char* layerName = nullptr);

public:
	const char* Name(uint32_t inx) const override;

	uint32_t Count() const override;

	std::string PickListName() const override;

};
//----------------------------------------------------------------

//---------------------------Instance----------------------------
class Instance
{
private:
	VkInstance m_instance;
	
	DebugReport m_debugReport;  // Configure debug report flags here.

private:
	void Create(const Layers& layers, const Extensions& extensions, const char* app_name, const char* engine_name);

public:
	Instance(const Layers& layers, const Extensions& extensions, const char* app_name = "PreVEngineApp", const char* engine_name = "PreVEngine");
	
	Instance(const bool enable_validation = true, const char* app_name = "PreVEngineApp", const char* engine_name = "PreVEngine");

	~Instance();
	
public:	
	void Print() const;

	DebugReport& GetDebugReport();

	operator VkInstance() const;
};
//----------------------------------------------------------------
#endif
