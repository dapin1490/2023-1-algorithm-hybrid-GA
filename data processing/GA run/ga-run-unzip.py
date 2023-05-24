import pandas as pd

def process_pool(data_route: str, title: str, to_save: str):
    data = pd.read_csv(data_route, header=0, index_col=0, encoding='utf-8')

    data0 = data.loc[data['continent'] == 0]
    data1 = data.loc[data['continent'] == 1]
    data2 = data.loc[data['continent'] == 2]

    data0.to_csv(to_save + f"{title}-0.csv")
    data1.to_csv(to_save + f"{title}-1.csv")
    data2.to_csv(to_save + f"{title}-2.csv")

    return

def unzip_pool(where_to_save: str, data_route: str, data_: pd.DataFrame = None):
    res = open(where_to_save, 'w', encoding='utf-8')
    data = data_
    if data_route is not None:
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

# # 50 노드 세대별 cost 통계
# save_to = r'20191130\data processing\basic GA\un50describe.csv'
# read_from = r'20191130\data processing\basic GA\un50pool.csv'
# unzip_pool(save_to, read_from)

# # 100 노드 세대별 cost 통계
# save_to = r'20191130\data processing\basic GA\un100describe.csv'
# read_from = r'20191130\data processing\basic GA\un100pool.csv'
# unzip_pool(save_to, read_from)

# # 500 노드 세대별 cost 통계
# save_to = r'20191130\data processing\basic GA\w500describe.csv'
# read_from = r'20191130\data processing\basic GA\w500pool.csv'
# unzip_pool(save_to, read_from)

# # island GA un 100 pool unzip
# data_route = r'20191130\data processing\island GA\un100pool.csv'
# to_save = r'20191130\data processing\island GA\\'
# title = 'un 100 continent'
# process_pool(data_route=data_route, title=title, to_save=to_save)

save_to = r'20191130\data processing\island GA\un-100-describe-cont0.csv'
read_from = r'20191130\data processing\island GA\un 100 continent-0.csv'
unzip_pool(save_to, read_from)

save_to = r'20191130\data processing\island GA\un-100-describe-cont1.csv'
read_from = r'20191130\data processing\island GA\un 100 continent-1.csv'
unzip_pool(save_to, read_from)

save_to = r'20191130\data processing\island GA\un-100-describe-cont2.csv'
read_from = r'20191130\data processing\island GA\un 100 continent-2.csv'
unzip_pool(save_to, read_from)