// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_KLIST_H
#define WAGTAIL_KLIST_H

namespace wagtail
{
	// Singly-linked list type:
	template<typename T> class klist
	{
		public:
		private:
			struct node
			{
				T data;
				node * next, * prev;
			};
	};
}

#endif

