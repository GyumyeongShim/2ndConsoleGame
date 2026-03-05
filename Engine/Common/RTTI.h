#pragma once

#include "Common/Common.h"

// RTTI를 선언할 클래스에 추가할 매크로.
// 아래 코드에서 Type, ParentType이 실제 타입으로 변환되어 복사/붙여넣기 됨.
#define RTTI_DECLARATIONS(Type, ParentType)												\
friend class RTTI;																		\
protected:																				\
	static const size_t TypeIdClass()													\
	{																					\
		static int runTimeTypeId = 0;													\
		return reinterpret_cast<size_t>(&runTimeTypeId);								\
	}																					\
public:																					\
	virtual const size_t& GetType() const override 										\
	{ 																					\
			return Type::TypeIdClass(); 												\
	}																					\
	using super = ParentType;															\
	virtual bool Is(const size_t id) const override										\
	{																					\
		if (id == TypeIdClass())														\
		{																				\
			return true;																\
		}																				\
		else																			\
		{																				\
			return ParentType::Is(id);													\
		}																				\
	}																					\
	virtual bool Is(RTTI* const rtti) const override									\
	{																					\
		return Is(rtti->GetType());														\
	}

namespace Wannabe
{
	// 상속 관계에 있는 클래스 간의
	// 동적(실행중에) 형변환을 하기 위해 사용하는 클래스.
	// RunTime-Type-Information.
	// 커스텀 RTTI를 제공하는 클래스의 최상의 클래스
	// C#의 'object', 언리얼엔진의 'UObject'와 비슷한 역할.
	class WANNABE_API RTTI
	{
	public:
		virtual const size_t& GetType() const = 0;

		virtual bool Is(RTTI* const rtti) const //여기까지 와서 형변환을 묻는거면 비교 자체가 무의미하기때문에 false 반환
		{
			return false;
		}

		virtual bool Is(const size_t id) const
		{
			return false;
		}

		//타입 질문 함수
		template<typename T>
		bool IsTypeOf()
		{
			return Is(T::TypeIdClass());
		}

		template<typename T>
		T* As()
		{
			if (Is(T::TypeIdClass()))
			{
				return (T*)this;
			}

			return nullptr;
		}

		template<typename T>
		const T* As() const
		{
			if (Is(T::TypeIdClass()))
			{
				return (T*)this;
			}

			return nullptr;
		}
	};

}
