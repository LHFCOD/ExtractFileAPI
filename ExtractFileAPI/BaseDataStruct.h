#pragma once
namespace BaseDataStruct
{
	typedef unsigned char byte_t;
	typedef unsigned short ui16_t;
	typedef unsigned int ui32_t;
	typedef int i32_t;
	typedef unsigned long long ui64_t;

	class FileBlock
	{
	public:
		byte_t *p = nullptr;
		int len = 0;
		FileBlock(byte_t *_p, int _len) {
			p = _p;
			len = _len;
		};
		FileBlock() {};
		~FileBlock() {
			if (p != nullptr)
			{
				free(p);
				p = nullptr;
			}
		};
	};
}
