import pandas as pd

def unzip_pool(where_to_save: str, data_route: str):
    res = open(where_to_save, 'w', encoding='utf-8')
    data = pd.read_csv(data_route, header=0, index_col=0, encoding='utf-8')

    count_pool = dict()  # 압축된 pool
    total_pool = dict()  # 압축 해제 pool
    max_len = 0

    # 가중치*개수 형태로 압축된 pool 데이터를 풀어서 새로운 데이터프레임으로 만들기
    for idx, row in zip(data.index, data['pool']):
        count_pool[idx] = row.split(' ')
        total_pool[idx] = []
        for i in range(len(count_pool[idx])):
            count_pool[idx][i] = count_pool[idx][i].split('*')
            total_pool[idx] += [int(count_pool[idx][i][0])] * int(count_pool[idx][i][1])
            if len(total_pool[idx]) > max_len:
                max_len = len(total_pool[idx])

    for i in total_pool.keys():
        total_pool[i] += [None] * (max_len - len(total_pool[i]))

    total_pool = pd.DataFrame.from_dict(total_pool)
    print(f"{total_pool.tail()}\n")

    # generation,count,mean,std,min,25%,50%,75%,max
    res.write(f"generation,count,mean,std,min,25%,50%,75%,max\n")
    for i in total_pool.columns:
        print(i, end=" ")
        icount = total_pool.loc[:, i:i].count().loc[i]
        imean = total_pool.loc[:, i:i].mean().loc[i]
        istd = total_pool.loc[:, i:i].std().loc[i]
        imin = total_pool.loc[:, i:i].min().loc[i]
        i25 = total_pool.loc[:, i:i].quantile(.25).loc[i]
        i50 = total_pool.loc[:, i:i].quantile(.5).loc[i]
        i75 = total_pool.loc[:, i:i].quantile(.75).loc[i]
        imax = total_pool.loc[:, i:i].max().loc[i]
        res.write(f"{i},{icount},{imean:.4f},{istd:.4f},{imin},{i25},{i50},{i75},{imax}\n")

    res.close()
    return

# 50 노드 세대별 cost 통계
save_to = r'20191130\data processing\basic GA\un50describe.csv'
read_from = r'20191130\data processing\basic GA\un50pool.csv'
unzip_pool(save_to, read_from)

# 100 노드 세대별 cost 통계
save_to = r'20191130\data processing\basic GA\un100describe.csv'
read_from = r'20191130\data processing\basic GA\un100pool.csv'
unzip_pool(save_to, read_from)

# 500 노드 세대별 cost 통계
save_to = r'20191130\data processing\basic GA\w500describe.csv'
read_from = r'20191130\data processing\basic GA\w500pool.csv'
unzip_pool(save_to, read_from)