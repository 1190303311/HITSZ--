#include <cstdio>
#include <cmath>
#include <ctime>
#include "pin.H"

/**************************************
 * Cache Model Base Class
**************************************/
class CacheModel
{
public:
    // Constructor
    CacheModel(UINT32 block_num, UINT32 log_block_size)
        : m_block_num(block_num), m_blksz_log(log_block_size),
          m_rd_reqs(0), m_wr_reqs(0), m_rd_hits(0), m_wr_hits(0)
    {
        m_valids = new bool[m_block_num];
        m_tags = new UINT32[m_block_num];
        m_replace_q = new UINT32[m_block_num];

        for (UINT i = 0; i < m_block_num; i++)
        {
            m_valids[i] = false;
            m_replace_q[i] = i;
        }
    }

    // Destructor
    virtual ~CacheModel()
    {
        delete[] m_valids;
        delete[] m_tags;
        delete[] m_replace_q;
    }

    // Update the cache state whenever data is read
    void readReq(UINT32 mem_addr)
    {
        m_rd_reqs++;
        if (access(mem_addr)) m_rd_hits++;
    }

    // Update the cache state whenever data is written
    void writeReq(UINT32 mem_addr)
    {
        m_wr_reqs++;
        if (access(mem_addr)) m_wr_hits++;
    }

    UINT32 getRdReq() { return m_rd_reqs; }
    UINT32 getWrReq() { return m_wr_reqs; }

    void dumpResults()
    {
        float rdHitRate = 100 * (float)m_rd_hits/m_rd_reqs;
        float wrHitRate = 100 * (float)m_wr_hits/m_wr_reqs;
        printf("\tread req: %lu,\thit: %lu,\thit rate: %.2f%%\n", m_rd_reqs, m_rd_hits, rdHitRate);
        printf("\twrite req: %lu,\thit: %lu,\thit rate: %.2f%%\n", m_wr_reqs, m_wr_hits, wrHitRate);
    }

protected:
    UINT32 m_block_num;     // The number of cache blocks
    UINT32 m_blksz_log;     // 块大小的对数

    bool* m_valids;
    UINT32* m_tags;
    UINT32* m_replace_q;    // Cache块替换的候选队列

    UINT64 m_rd_reqs;       // The number of read-requests
    UINT64 m_wr_reqs;       // The number of write-requests
    UINT64 m_rd_hits;       // The number of hit read-requests
    UINT64 m_wr_hits;       // The number of hit write-requests

    // Look up the cache to decide whether the access is hit or missed
    virtual bool lookup(UINT32 mem_addr, UINT32& blk_id) = 0;

    // Access the cache: update m_replace_q if hit, otherwise replace a block and update m_replace_q
    virtual bool access(UINT32 mem_addr) = 0;

    // Update m_replace_q
    virtual void updateReplaceQ(UINT32 blk_id) = 0;
};

/**************************************
 * Fully Associative Cache Class
**************************************/
class FullAssoCache : public CacheModel
{
public:
    // Constructor
    FullAssoCache(UINT32 block_num, UINT32 log_block_size)
        : CacheModel(block_num, log_block_size) {}

    // Destructor
    ~FullAssoCache() {}

private:
    UINT32 getTag(UINT32 addr) { return addr >> m_blksz_log;/* TODO */ }

    // Look up the cache to decide whether the access is hit or missed
    bool lookup(UINT32 mem_addr, UINT32& blk_id)
    {
        UINT32 tag = getTag(mem_addr);

        // TODO
	for(UINT32 i=0; i<m_block_num; i++)
	{
		if((m_tags[i] == tag) && m_valids[i])
		{
			blk_id = i;
			return true;
		}
	}

        return false;
    }

    // Access the cache: update m_replace_q if hit, otherwise replace a block and update m_replace_q
    bool access(UINT32 mem_addr)
    {
        UINT32 blk_id;
        if (lookup(mem_addr, blk_id))
        {
            updateReplaceQ(blk_id);     // Update m_replace_q
            return true;
        }

        // Get the to-be-replaced block id using m_replace_q
        UINT32 bid_2be_replaced = m_replace_q[0];// TODO

        // Replace the cache block
        // TODO
	m_tags[bid_2be_replaced] = getTag(mem_addr);
	m_valids[bid_2be_replaced] = true;
        updateReplaceQ(bid_2be_replaced);

        return false;
    }

    // Update m_replace_q
    void updateReplaceQ(UINT32 blk_id)
    {
        // TODO
	for(UINT32 i = 0; i<m_block_num; i++)
	{
		if(m_replace_q[i] == blk_id)
		{
			for(UINT32 j = i; j<m_block_num-1; j++)
				m_replace_q[j] = m_replace_q[j+1];
			m_replace_q[m_block_num-1] = blk_id;
			break;
 		}
	}
    }
};

/**************************************
 * Directly Mapped Cache Class
**************************************/
class DirectMapCache : public CacheModel
{
public:
    // Constructor
    DirectMapCache(UINT32 block_num, UINT32 log_block_size)
        : CacheModel(block_num, log_block_size) {}

    // Destructor
    ~DirectMapCache() {}

private:

    // 
	UINT32 getTag(UINT32 addr) { return addr >> (m_blksz_log + UINT32(log2(m_block_num)));/* TODO */ }
	UINT32 getBlk_num(UINT32 addr) { return (addr >> m_blksz_log) & (m_block_num-1);/* TODO */ }
    // Look up the cache to decide whether the access is hit or missed
    bool lookup(UINT32 mem_addr, UINT32& blk_id)
    {
        // TODO
	UINT32 tag = getTag(mem_addr);
	UINT32 blk_num = getBlk_num(mem_addr);
	if((m_tags[blk_num] == tag) && m_valids[blk_num])
	{
		blk_id = blk_num;
		return true;
	}
	return false;
    }

    // Access the cache: update m_replace_q if hit, otherwise replace a block and update m_replace_q
    bool access(UINT32 mem_addr)
    {
        // TODO
	UINT32 blk_id;
	if(lookup(mem_addr, blk_id))
	{
		updateReplaceQ(blk_id);
		return true;
	}
	m_tags[getBlk_num(mem_addr)] = getTag(mem_addr);
	m_valids[getBlk_num(mem_addr)] = true;
	updateReplaceQ(getBlk_num(mem_addr));
	return false;
    }

    // Update m_replace_q
    void updateReplaceQ(UINT32 blk_id)
    {
        // TODO: do nothing
    }
};

/**************************************
 * Set-Associative Cache Class
**************************************/
class SetAssoCache : public CacheModel
{
public:
	 UINT32 m_sets_log;
	 UINT32 m_blksz_log;
	 UINT32 m_ass;
    // Constructor
    SetAssoCache(/* TODO */UINT32 sets_log, UINT32 log_blk_size, UINT32 ass): CacheModel((1<<sets_log)*ass, log_blk_size)
    {
	m_sets_log = sets_log;
	m_blksz_log = log_blk_size;
	m_ass = ass;
    }

    // Destructor
    ~SetAssoCache() {}

private:

    // 
	UINT32 getTag(UINT32 addr) { return addr >> (m_blksz_log + m_sets_log);/* TODO */ }
	UINT32 getSet_num(UINT32 addr) { return (addr >> m_blksz_log) & ((1<<m_sets_log)-1);/* TODO */ }
    // Look up the cache to decide whether the access is hit or missed
    bool lookup(UINT32 mem_addr, UINT32& blk_id)
    {
        // TODO
	UINT32 tag = getTag(mem_addr);
	UINT32 set_num = getSet_num(mem_addr);
	for(UINT i=0; i<m_ass; i++)
	{
		if((m_tags[set_num*m_ass+i] == tag) && m_valids[set_num*m_ass+i])
		{
			blk_id = set_num*m_ass+i;
			return true;
		}
	}
	return false;
    }

    // Access the cache: update m_replace_q if hit, otherwise replace a block and update m_replace_q
    bool access(UINT32 mem_addr)
    {
        // TODO
	UINT32 tag = getTag(mem_addr);
	UINT32 set_num = getSet_num(mem_addr);
	UINT32 blk_id;
	if(lookup(mem_addr, blk_id))
	{
		updateReplaceQ(blk_id);
		return true;
	}
	UINT32 bid_2be_replaced = m_replace_q[set_num*m_ass+0];
	m_tags[bid_2be_replaced] = tag;
	m_valids[bid_2be_replaced] = true;
	updateReplaceQ(bid_2be_replaced);
	return false;
    }

    // Update m_replace_q
    void updateReplaceQ(UINT32 blk_id)
    {
        // TODO
	UINT32 set_num = blk_id/m_ass;
	for(UINT32 i = 0; i<m_ass; i++)
	{
		if(m_replace_q[set_num*m_ass+i] == blk_id)
		{
			for(UINT32 j = i; j<m_ass-1; j++)
				m_replace_q[set_num*m_ass+j] = m_replace_q[set_num*m_ass+j+1];
			m_replace_q[set_num*m_ass+m_ass-1] = blk_id;
			break;
 		}
	}
    }
};

CacheModel* my_fa_cache;
CacheModel* my_dm_cache;
CacheModel* my_sa_cache;

double time_fa_rd = 0, time_fa_wr = 0;
double time_dm_rd = 0, time_dm_wr = 0;
double time_sa_rd = 0, time_sa_wr = 0;

// Cache reading analysis routine
void readCache(UINT32 mem_addr)
{
    mem_addr = (mem_addr >> 2) << 2;
    clock_t pt0 = clock();
    my_fa_cache->readReq(mem_addr);
    clock_t pt1 = clock();
    my_dm_cache->readReq(mem_addr);
    clock_t pt2 = clock();
    my_sa_cache->readReq(mem_addr);
    clock_t pt3 = clock();

    time_fa_rd += 1000000*(double)(pt1 - pt0) / CLOCKS_PER_SEC;
    time_dm_rd += 1000000*(double)(pt2 - pt1) / CLOCKS_PER_SEC;
    time_sa_rd += 1000000*(double)(pt3 - pt2) / CLOCKS_PER_SEC;
}

// Cache writing analysis routine
void writeCache(UINT32 mem_addr)
{
    mem_addr = (mem_addr >> 2) << 2;
    clock_t pt0 = clock();
    my_fa_cache->writeReq(mem_addr);
    clock_t pt1 = clock();
    my_dm_cache->writeReq(mem_addr);
    clock_t pt2 = clock();
    my_sa_cache->writeReq(mem_addr);
    clock_t pt3 = clock();

    time_fa_wr += 1000000*(double)(pt1 - pt0) / CLOCKS_PER_SEC;
    time_dm_wr += 1000000*(double)(pt2 - pt1) / CLOCKS_PER_SEC;
    time_sa_wr += 1000000*(double)(pt3 - pt2) / CLOCKS_PER_SEC;
}

// This knob will set the cache param m_block_num
KNOB<UINT32> KnobBlockNum(KNOB_MODE_WRITEONCE, "pintool",
        "n", "512", "specify the number of blocks in bytes");

// This knob will set the cache param m_blksz_log
KNOB<UINT32> KnobBlockSizeLog(KNOB_MODE_WRITEONCE, "pintool",
        "b", "6", "specify the log of the block size in bytes");

// This knob will set the cache param m_sets_log
KNOB<UINT32> KnobSetsLog(KNOB_MODE_WRITEONCE, "pintool",
        "r", "7", "specify the log of the number of rows");

// This knob will set the cache param m_asso
KNOB<UINT32> KnobAssociativity(KNOB_MODE_WRITEONCE, "pintool",
        "a", "4", "specify the m_asso");

// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)
{
    if (INS_IsMemoryRead(ins))
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)readCache, IARG_MEMORYREAD_EA, IARG_END);
    if (INS_IsMemoryWrite(ins))
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)writeCache, IARG_MEMORYWRITE_EA, IARG_END);
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    printf("\nFully Associative Cache:\n");
    printf("average read time: %.2fus\n", time_fa_rd/my_fa_cache->getRdReq());
    printf("average write time: %.2fus\n", time_fa_rd/my_fa_cache->getWrReq());
    my_fa_cache->dumpResults();
    printf("\nDirectly Mapped Cache:\n");
    printf("average read time: %.2fus\n", time_dm_rd/my_dm_cache->getRdReq());
    printf("average write time: %.2fus\n", time_dm_rd/my_dm_cache->getWrReq());
    my_dm_cache->dumpResults();
    printf("\nSet-Associative Cache:\n");
    printf("average read time: %.2fus\n", time_sa_rd/my_sa_cache->getRdReq());
    printf("average write time: %.2fus\n", time_sa_rd/my_sa_cache->getWrReq());
    my_sa_cache->dumpResults();

    delete my_fa_cache;
    delete my_dm_cache;
    delete my_sa_cache;
}

// argc, argv are the entire command line, including pin -t <toolname> -- ...
int main(int argc, char* argv[])
{
    // Initialize pin
    PIN_Init(argc, argv);

    my_fa_cache = new FullAssoCache(KnobBlockNum.Value(), KnobBlockSizeLog.Value());
    my_dm_cache = new DirectMapCache(KnobBlockNum.Value(), KnobBlockSizeLog.Value());
    my_sa_cache = new SetAssoCache(KnobSetsLog.Value(), KnobBlockSizeLog.Value(), KnobAssociativity.Value());

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);

    // Start the program, never returns
    PIN_StartProgram();

    return 0;
}
