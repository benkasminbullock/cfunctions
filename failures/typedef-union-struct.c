#ifdef HEADER
typedef union Ins {
    struct {
	byte	tag;
	byte	marked;
	void	*link;
    }			i;
    struct {
	unsigned short	value;
	unsigned short	bump;
	void	*link;
    }			c;
} Ins;
#endif /* def HEADER */

