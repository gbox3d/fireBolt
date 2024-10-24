#%%
import numpy as np
import pandas as pd


#%%
def read_and_process_data(filename):
    with open(filename, "rb") as f:
        data = f.read()
    
    # 바이트 데이터를 비트 배열로 변환
    bit_array = np.unpackbits(np.frombuffer(data, dtype=np.uint8))
    
    # 8비트 단위로 순서를 뒤집습니다
    bit_array = bit_array.reshape(-1, 8)[:, ::-1].ravel()
    
    # 채널별로 데이터 재구성
    channels = bit_array.reshape(-1, 8)
    
    return channels

#%%
def process_channel_data(channels, channel_index=0, min_set_size=10, distance_threshold=100):
    """
    채널 데이터를 처리하여 거리 세트를 생성하는 함수
    
    :param channels: 채널 데이터
    :param channel_index: 처리할 채널의 인덱스 (기본값: 0)
    :param min_set_size: 유지할 최소 세트 크기 (기본값: 10)
    :param distance_threshold: 새 세트를 시작할 거리 임계값 (기본값: 100)
    :return: 거리 세트의 리스트
    """
    _datas = channels[:, channel_index]

    indices = []
    for i, data in enumerate(_datas):
        if data != 0:
            indices.append(i)

    data_set = []
    set_index = 0
    data_set.append([])

    for i in range(1, len(indices)):
        _distance = indices[i] - indices[i-1]
        if _distance > distance_threshold:
            # print(f"DISTANCE: {_distance}")
            set_index += 1
            data_set.append([])
        else:
            data_set[set_index].append(_distance)

    # min_set_size 이상의 데이터가 있는 세트만 유지
    data_set = [sublist for sublist in data_set if len(sublist) >= min_set_size]

    return data_set


def export_to_csv(data_set, filename='output.csv'):
    """
    데이터 세트를 CSV 파일로 출력하는 함수
    
    :param data_set: 출력할 데이터 세트
    :param filename: 출력할 CSV 파일 이름 (기본값: 'output.csv')
    """
    # 데이터프레임 생성을 위한 딕셔너리
    df_dict = {}
    
    # 가장 긴 리스트의 길이 찾기
    max_length = max(len(sublist) for sublist in data_set)
    
    # 각 세트를 데이터프레임의 열로 변환
    for i, sublist in enumerate(data_set):
        column_name = f'Set {i+1}'
        # 리스트의 길이를 최대 길이에 맞추기 위해 빈 값으로 채우기
        df_dict[column_name] = sublist + [None] * (max_length - len(sublist))
    
    # 데이터프레임 생성
    df = pd.DataFrame(df_dict)
    
    # CSV 파일로 출력
    df.to_csv(filename, index=False)
    print(f"데이터가 {filename} 파일로 저장되었습니다.")

#%%
if __name__ == "__main__":
    
    # 데이터 읽기 및 처리
    filename = "capture_003.bin"
    data_set = process_channel_data(read_and_process_data(filename), channel_index=0, min_set_size=10, distance_threshold=100)
    
    print(f"data load complete. data_set length: {len(data_set)}")
    
#%%
    export_to_csv(data_set, 'channel_data_output.csv')

#%%
    import matplotlib.pyplot as plt
    # 박스 플롯 그리기
    plt.figure(figsize=(12, 6))
    plt.boxplot(data_set, labels=[f'Set {i+1}' for i in range(len(data_set))])
    plt.title('Distribution of Distances in Each Set')
    plt.xlabel('Sets')
    plt.ylabel('Distance')
    plt.grid(True)
    plt.show()

    # 각 세트의 기본 통계 출력
    for i, sublist in enumerate(data_set):
        print(f"Set {i+1}:")
        print(f"  Min: {min(sublist)}")
        print(f"  Max: {max(sublist)}")
        print(f"  Mean: {np.mean(sublist):.2f}")
        print(f"  Median: {np.median(sublist):.2f}")
        print(f"  Standard Deviation: {np.std(sublist):.2f}")
        print(f"  length: {len(sublist)}")
        print()

    # 히스토그램 그리기
    plt.figure(figsize=(12, 6))
    for i, sublist in enumerate(data_set):
        plt.hist(sublist, bins=20, alpha=0.5, label=f'Set {i+1}')
    plt.title('Histogram of Distances in Each Set')
    plt.xlabel('Distance')
    plt.ylabel('Frequency')
    plt.legend()
    plt.grid(True)
    plt.show()
    

# %%
