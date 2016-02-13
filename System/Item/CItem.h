#ifndef INC_CITEM_H
#define INC_CITEM_H


#include <System/Item/CItemProperties.h>

#include <memory>

using std::shared_ptr;


class CItem
{
	friend class CItemHandler;
	friend class CInventory;

public: //datatype
	using Datatype = shared_ptr<CItem>;


private: //constructor / destructor
	CItem(const string &name, const CItemProperties &type, const unsigned int dbid) :
		Type(name, type),
		DataId(dbid)
	{}
	~CItem() = default;

private: //variables
	bool m_Invalidated = false;


public: //variables
	const unsigned int DataId = 0;
	struct sm_Type
	{
		sm_Type(const string &name, const CItemProperties &type) :
			Name(name),
			Data(type)
		{}
		
		const string Name;
		const CItemProperties &Data;
	} Type;

	CItemProperties Data;


public: //functions
	inline bool Valid() const
	{
		return m_Invalidated == false && 
			DataId != 0 && Type.Name.empty() == false && 
			Type.Data && Data;
	}
	inline void Invalidate()
	{
		m_Invalidated = true;
	}

	void Save();
	void Destroy();


public: //assignment functions
	CItem(const CItem &rhs) = delete;
	void operator= (const CItem &rhs) = delete;


private: //fabricator function
	static Datatype Create(
		const string &name, 
		const CItemProperties &type, 
		const unsigned int dbid)
	{
		return Datatype(new CItem(name, type, dbid),
			[](CItem *i) { i->Destroy(); });
	}

};


#endif // !INC_CITEM_H
