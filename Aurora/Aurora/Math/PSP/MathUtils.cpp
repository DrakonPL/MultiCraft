#include <Aurora/Math/MathUtils.h>

#include <math.h>
#include <psptypes.h>

#define PII   3.1415926535897932384626433832795f
#define PI_2 1.5707963267948966192313216916398f

namespace Aurora
{
	namespace Math
	{
		float ASinf(float s)
		{
			float result;
			__asm__ (
			".set			push\n"					// save assembler option
			".set			noreorder\n"			// suppress reordering
			"lv.s			s000, %1\n"				// s000 = s
			"vcst.s			s001, VFPU_2_PI\n"		// s001 = VFPU_2_PI = 2 / PI
			"vmul.s			s000, s000, s001\n"		// s000 = s000 * s001
			"vsin.s			s000, s000\n"			// s000 = sin(s000)
			"sv.s			s000, %0\n"				// d    = s000
			".set			pop\n"					// restore assembler option
			: "=m"(result)
			: "m"(s)
			);
			return result;
		}

		float AASinf(float s)
		{
			float d;
			__asm__ (
				".set			push\n"					// save assembler option
				".set			noreorder\n"			// suppress reordering
				"lv.s			s000, %1\n"				// s000 = s
				"vcst.s			s001, VFPU_SQRT1_2\n"	// s001 = VFPU_SQRT1_2 = 1 / sqrt(2)
				"vcst.s			s002, VFPU_PI_2\n"		// s002 = VFPU_PI_2    = PI / 2
				"vcmp.s			LT, s000[|x|], s001\n"	// CC[0] = |s000| < s001
				"vmul.s			s003, s000, s000\n"		// s003 = s000 * s000
				"vsgn.s			s001, s000\n"			// s001 = sign(s000)
				"bvtl			0, 0f\n"				// if (CC[0]!=0) goto 0f
				"vasin.s		s000, s000\n"			// s000 = asin(s000)

				"vocp.s			s003, s003\n"			// s003 = 1.0 - s003
				"vsqrt.s		s003, s003\n"			// s003 = sqrt(s003)
				"vmul.s			s002, s002, s001\n"		// s002 = s002 * s001
				"vasin.s		s003, s003\n"			// s003 = asin(s003)
				"vocp.s			s000, s003\n"			// s000 = 1.0 - s003
			"0:\n"
				"vmul.s			s000, s000, s002\n"		// s000 = s000 * s002
				"sv.s			s000, %0\n"				// d    = s000
				".set			pop\n"					// restore assembler option
				: "=m"(d)
				: "m"(s)
			);
			return d;
		}

		float ACosf(float s)
		{
			return cosf(s);
		}

		float AACosf(float s)
		{
			float result;
		   __asm__ (
				".set			push\n"					// save assembler option
				".set			noreorder\n"			// suppress reordering
				"lv.s			s000, %1\n"				// s000 = s
				"vcst.s			s001, VFPU_SQRT1_2\n"	// s001 = VFPU_SQRT1_2 = 1 / sqrt(2)
				"vcst.s			s002, VFPU_PI_2\n"		// s002 = VFPU_PI_2    = PI / 2
				"vcmp.s			LT, s000[|x|], s001\n"	// CC[0] = |s000| < s001
				"vmul.s			s003, s000, s000\n"		// s003 = s000 * s000
				"vsgn.s			s001, s000\n"			// s001 = sign(s000)
				"bvtl			0, 0f\n"				// if (CC[0]!=0) goto 0f
				"vasin.s		s000, s000\n"			// s000 = asin(s000)

				"vocp.s			s003, s003\n"			// s003 = 1.0 - s003
				"vsqrt.s		s003, s003\n"			// s003 = sqrt(s003)
				"vasin.s		s003, s003\n"			// s003 = asin(s003)
				"vocp.s			s000, s003\n"			// s000 = 1.0 - s003
				"vmul.s			s000, s000, s001\n"		// s000 = s000 * s001
			"0:\n"
				"vocp.s			s000, s000\n"			// s000 = 1.0 - s000
				"vmul.s			s000, s000, s002\n"		// s000 = s000 * s002
				"sv.s			s000, %0\n"				// d    = s000
				".set			pop\n"					// restore assembler option
				: "=m"(result)
				: "m"(s)
			);
			return result;
		}

		float ASqrtf(float s)
		{
			float result;
			__asm__ (
				".set			push\n"					// save assembler option
				".set			noreorder\n"			// suppress reordering
				"lv.s			s000, %1\n"				// s000 = s
				"vsqrt.s		s000, s000\n"			// s000 = sqrt(s000)
				"sv.s			s000, %0\n"				// d    = s000
				".set			pop\n"					// restore assembler option
				: "=m"(result)
				: "m"(s)
			);
			return result;
		}

		float ACeilf(float s)
		{
			float d;

			__asm__ (
				".set			push\n"					// save assembler option
				".set			noreorder\n"			// suppress reordering
				"lv.s			s000, %1\n"				// s000 = s
				"vf2iu.s		s000, s000, 0\n"		// s000 = (int)ceil(s000)
				"vi2f.s			s000, s000, 0\n"		// s000 = (float)s000
				"sv.s			s000, %0\n"				// d    = s000
				".set			pop\n"					// restore assembler option
				: "=m"(d)
				: "m"(s)
			);

			return (d);
		}

		float AFloorf(float s)
		{
			float d;

			__asm__ (
				".set			push\n"					// save assembler option
				".set			noreorder\n"			// suppress reordering
				"lv.s			s000, %1\n"				// s000 = s
				"vf2id.s		s000, s000, 0\n"		// s000 = (int)floor(s000)
				"vi2f.s			s000, s000, 0\n"		// s000 = (float)s000
				"sv.s			s000, %0\n"				// d    = s000
				".set			pop\n"					// restore assembler option
				: "=m"(d)
				: "m"(s)
			);

			return (d);
		}
	}
}
