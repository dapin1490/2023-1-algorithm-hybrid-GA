import pandas as pd

def process_pool(data_route: str, title: str, to_save: str):
    data = pd.read_csv(data_route, header=0, index_col=0, encoding='utf-8')

    data0 = data.loc[data['continent'] == 0]
    data1 = data.loc[data['continent'] == 1]
    data2 = data.loc[data['continent'] == 2]

    data0.to_csv(to_save + f"{title}-0.csv")
    data1.to_csv(to_save + f"{title}-1.csv")
    data2.to_csv(to_save + f"{title}-2.csv")

    print(f'{title} processed')

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
    # print(f"{total_pool.tail()}\n")

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
    print(f'\nunziped \'{data_route}\'')
    return

def connect_pool(route_base: str, title: str):
    cont0 = pd.read_csv(route_base + title + '-describe-cont0.csv', header=0, index_col=0, encoding='utf-8')
    cont1 = pd.read_csv(route_base + title + '-describe-cont1.csv', header=0, index_col=0, encoding='utf-8')
    cont2 = pd.read_csv(route_base + title + '-describe-cont2.csv', header=0, index_col=0, encoding='utf-8')

    cont_total = pd.concat([cont0, cont1, cont2])
    cont_total.reset_index(inplace=True, drop=True)
    print(cont_total.tail())

    cont_total.to_csv(route_base + title + '-total.csv')
    print(f'connected {title} saved')
    return

# basic GA
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

# island GA pools unzip
names = ['un50', 'un100', 'w500', 'wc297']
route_base = r'data processing/island GA/'

# for name in names:
#     data_route = route_base + 'pool data/' + name + 'pool.csv'
#     title = name + '-continent'
#     process_pool(data_route=data_route, title=title, to_save=route_base + r'continents/')
#     for i in [0, 1, 2]:
#         save_to = route_base + 'describe-cont/' + name + f'-describe-cont{i}.csv'
#         read_from = route_base + r'continents/' + name + f'-continent-{i}.csv'
#         unzip_pool(save_to, read_from)

for name in names:
    connect_pool(route_base=route_base + 'describe-cont/', title=name)