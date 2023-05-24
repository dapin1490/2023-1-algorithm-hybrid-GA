import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv(r'20191130\data processing\basic GA\un100describe.csv', header=0, index_col=0, encoding='utf-8')

# 개별 그래프 그리기
for col in data.columns:
    print(col)

    plt.figure(figsize=(12, 8))  # 그래프 크기
    plt.grid(alpha=0.3)  # 그리드 투명도

    plt.plot(data.index, data[col], alpha=0.8)

    plt.title(f"un100 {col}")  # 그래프 제목에 괄호 치고 분산 표기
    # plt.ylim(data[col].min() // 10 * 10 - 10, data[col].max() // 10 * 10 + 10)  # y 값 범위 제한

    plt.xlabel('generation')
    plt.ylabel(col, rotation='horizontal')

    plt.savefig(rf'20191130\data processing\basic GA\images\un100{col}.jpg', bbox_inches='tight')
    plt.clf()

columns = ['count', 'mean', 'std', 'min', '25%', '50%', '75%', 'max']

# 전체 pool 변화 그리기
def draw_gen_change(data_route: str, title: str, save_route: str):
    data = pd.read_csv(data_route, header=0, index_col=0, encoding='utf-8')
    x = data.index

    plt.figure(figsize=(20, 12))
    plt.grid(alpha=0.3)

    plt.plot(x, data['min'], alpha=0.8)
    plt.plot(x, data['25%'], alpha=0.8)
    plt.plot(x, data['50%'], alpha=0.8)
    plt.plot(x, data['75%'], alpha=0.8)
    plt.plot(x, data['max'], alpha=0.8)

    plt.title(title, fontsize=20)
    plt.legend(['min', '25%', '50%', '75%', 'max'], loc='upper left', fontsize=15)

    plt.xlabel('generation', fontsize=20)
    plt.ylabel('cost', fontsize=20)

    plt.savefig(save_route, bbox_inches='tight')
    print(f"saved {title}")
    return

draw_gen_change(data_route=r'20191130\data processing\basic GA\un50describe.csv',
                title=f"basic GA unweighted 50 generational change",
                save_route=r'20191130\data processing\basic GA\images\basic-GA-un50-generational-change.jpg')

draw_gen_change(data_route=r'20191130\data processing\basic GA\un100describe.csv',
                title=f"basic GA unweighted 100 generational change",
                save_route=r'20191130\data processing\basic GA\images\basic-GA-un100-generational-change.jpg')

draw_gen_change(data_route=r'20191130\data processing\basic GA\w500describe.csv',
                title=f"basic GA weighted 500 generational change",
                save_route=r'20191130\data processing\basic GA\images\basic-GA-w500-generational-change.jpg')
