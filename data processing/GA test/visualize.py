import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.container import BarContainer
from io import TextIOWrapper

# bar 그래프의 막대 위에 값 쓰기
def bar_caption(_bar: BarContainer) -> BarContainer:
    for rect in _bar:
        height = rect.get_height()
        plt.text(rect.get_x() + rect.get_width()/2.0, height, f'{height}', ha='center', va='bottom', size = 10)  # 'top', 'bottom', 'center', 'baseline', 'center_baseline'
    return _bar

# 주어진 데이터를 정렬하고 바 그래프와 통계 텍스트 데이터로 저장함.
def save_stat(data_route: str, file: TextIOWrapper, fig_route: str, title: str, fgsize: tuple=(12, 8)):
    data = pd.read_csv(data_route, header=0, index_col=0, encoding='utf-8')  # 데이터 불러오기
    data.sort_values(by='cost', inplace=True)  # 오름차순 정렬
    data.reset_index(inplace=True, drop=True)  # 인덱스 초기화

    # 통계 텍스트 파일 작성
    file.write(f"{title}\n")  # 제목
    file.write(f"{str(data.describe())}\n\n")  # 통계

    # 바 그래프 작성
    plt.figure(figsize=fgsize)  # 그래프 크기
    plt.grid(alpha=0.3)  # 그리드 투명도
    bar = plt.bar(data.index, data['cost'], align='center', alpha=0.8)  # 바 그래프 생성
    bar = bar_caption(bar)  # 그래프 막대 위에 값 쓰기

    # 바 그래프 위에 평균선 긋기
    plt.plot(data.index, [data['cost'].mean()]*len(data.index), 'm--', alpha=0.5)  # 마젠타색 점선, 투명도 0.5
    plt.legend([f'mean: {data["cost"].mean():.2f}'], loc='upper left')  # 그래프 왼쪽 위에 범례

    plt.title(f"{title}(std: {data['cost'].std():.2f})")  # 그래프 제목에 괄호 치고 분산 표기
    plt.ylim(data['cost'].min() // 10 * 10 - 10, data['cost'].max() // 10 * 10 + 10)  # y 값 범위 제한
    plt.yticks(rotation=0)

    plt.savefig(fig_route, bbox_inches='tight')  # 그래프 저장
    return

res = open(r'20191130/data processing/basic GA/test-result.txt', 'w', encoding='utf-8')
print(type(res))

# 노드 50개 테스트 결과
save_stat(data_route=r"20191130/basic GA/res/un50test.csv",
          file=res,
          fig_route=r'20191130/data processing/basic GA/images/un50test-graph.jpg',
          title="unweighted 50")

# 노드 100개 테스트 결과
save_stat(data_route=r"20191130/basic GA/res/un100test.csv",
          file=res,
          fig_route=r'20191130/data processing/basic GA/images/un100test-graph.jpg',
          title="unweighted 100")

# 노드 500개 테스트 결과
save_stat(data_route=r"20191130/basic GA/res/w500test.csv",
          file=res,
          fig_route=r'20191130/data processing/basic GA/images/w500test-graph.jpg',
          title="weighted 500")

res.close()