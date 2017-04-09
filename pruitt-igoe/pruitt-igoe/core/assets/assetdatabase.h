#ifndef ASSETDATABASE_H
#define ASSETDATABASE_H

#include "../../common.h"
#include "asset.h"

#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <vector>

class AssetDatabase
{
public:
    static AssetDatabase * GetInstance();

    template <class T>
    T* LoadAsset(const std::string& id)
    {
        T * t = FindAsset<T>(id);

        if(t != nullptr)
            return t;

        t = new T();
        t->LoadFromFilename(id);
        assetMap[typeid(T)][id] = t;
        return t;
    }

	// For parameters
	template <class T, class P>
	T* LoadAsset(const std::string& id, const P& p)
	{
		T * t = FindAsset<T>(id);

		if (t != nullptr)
			return t;

		t = new T(p);
		t->LoadFromFilename(id);
		assetMap[typeid(T)][id] = t;
		return t;
	}

    template<class T>
    T * FindAsset(std::string id)
    {
        std::unordered_map<std::string, Asset*>& idMap = assetMap[typeid(T)];
		auto value = idMap.find(id);

        if(value != idMap.end())
            return dynamic_cast<T*>(value->second);

        return nullptr;
    }

private:
	static AssetDatabase * instance;
	std::unordered_map<std::type_index, std::unordered_map<std::string, Asset*>> assetMap;
	AssetDatabase();
};

#endif // ASSETDATABASE_H
