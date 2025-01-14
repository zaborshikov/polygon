#include iostream
#include cmath
#include chrono
#include fstream
#include string
#include sstream
#include vector
#include mutex
#include thread
#include atomic
#include set
#include chrono

#define ARMA_WARN_LEVEL 1
#define MLPACK_PRINT_INFO
#define MLPACK_PRINT_WARN
#include mlpack.hpp
#include modelling.cpp

stdmutex cout_mutex;

 Функция для бинаризации данных
armamat Binarize(const armamat& dataset, const stdvectordouble& thresholds, int cols) {
    armamat binary_dataset = dataset;
    for (int j = 0; j  cols; ++j) {
        binary_dataset.row(j) = armaconv_toarmamatfrom(binary_dataset.row(j)  thresholds[j]);
    }
    return binary_dataset;
}

double feature_selection(const int& train_cols = 11, const int& offset = 30000, bool print_best = false) {
    const char path = CUsersa1az1DownloadsWineQT.csv;
    const int cols = 11;
    const int shift = cols - train_cols;
    const int ntrasholds = 5;
    double time_elapsed_global = 0;


    armamat dataset;
    if (!mlpackdataLoad(path, dataset)) {
        throw stdruntime_error(Could not read .csv!);
    }

    if (dataset.is_empty()) {
        throw stdruntime_error(Loaded dataset is empty!);
    }

    size_t num_rows = dataset.n_cols;
    size_t num_rows_percent = static_castsize_t(stdceil(num_rows  1));

    armamat selectedrows = dataset.cols(0, num_rows_percent - 1);
    armamat subset = selectedrows.rows(shift, cols + 1);  + 2 (индексы + y) - 1 (все таки индекс, а не количество) = + 1



    stdcout  Dataset shape   subset.n_rows  x  subset.n_cols  stdendl;

     Предварительное вычисление пороговых значений
    stdvectorstdvectordouble thresholds(train_cols, stdvectordouble(ntrasholds));
    for (int j = 0; j  train_cols; ++j) {
        double min_val = subset.row(j).min();
        double max_val = subset.row(j).max();
        double range = (max_val - min_val)  (ntrasholds + 1);
        for (int s = 1; s = ntrasholds; ++s) {
            thresholds[j][s - 1] = min_val + range  s;
        }
    }

    double best_score = INFINITY;
    unsigned int best_idx;
    auto t_start = stdchronohigh_resolution_clocknow();

    auto iters = static_castlong long(stdpow(ntrasholds, train_cols));
    stdatomiclong long iteration_counter = 0;

    const int num_threads = stdthreadhardware_concurrency();
    stdvectorstdthread threads;

    auto worker = [&](int thread_id) {
        armamat post_dataset;
        double local_best_score = INFINITY;
        int local_best_idx = -1;

        for (long long i = thread_id; i  iters; i += num_threads) {
             Вычисляем текущее сочетание порогов
            stdvectordouble current_thresholds(train_cols);
            long long idx = i;

            for (int j = 0; j  train_cols; ++j) {
                int step = idx % ntrasholds;
                current_thresholds[j] = thresholds[j][step];
                idx = ntrasholds;
            }

             Бинаризуем данные
            post_dataset = Binarize(subset, current_thresholds, train_cols);

             Оцениваем модель
            double score = evaluate_dataset(post_dataset, 11 - shift, 12 - shift);

             Обновляем локальный лучший результат
            if (score  local_best_score) {
                local_best_score = score;
                local_best_idx = i;
            }

             Обновляем общий лучший результат (без гонок данных)
            {
                stdlock_guardstdmutex lock(cout_mutex);
                if (local_best_score  best_score) {
                    best_score = local_best_score;
                    best_idx = local_best_idx;
                    if (print_best) stdcout  New best score! IDX   best_idx  , RMSE   best_score  'n';
                }
            }

            ++iteration_counter;

             Периодический вывод прогресса
            if (iteration_counter % offset == 0) {
                auto elapsed_seconds = stdchronohigh_resolution_clocknow() - t_start;
                double progress = static_castdouble(iteration_counter)  iters;
                double time_left = (elapsed_seconds.count()  progress)  (1 - progress);

                {
                    stdlock_guardstdmutex lock(cout_mutex);
                    stdcout  Thread ID   thread_id
                          (  progress  100  %) Iteration   iteration_counter
                          RMSE   score
                          Best RMSE   best_score
                          Elapsed Time   elapsed_seconds.count()  1e9
                          s, Time Left   time_left  1e9   sn;
                }
            }
        }
    };

    for (int t = 0; t  num_threads; ++t) {
        threads.emplace_back(worker, t);
    }

    for (auto& thread  threads) {
        thread.join();
    }

    auto elapsed_seconds = (stdchronohigh_resolution_clocknow() - t_start).count()  1e9;


    stdcout  Best trasholds ;
    for (int j = 0; j  train_cols; ++j) {
        int step = best_idx % ntrasholds;
        stdcout  step;
        best_idx = ntrasholds;
    }
    stdcout  'n'  Bеst RMSE   best_score  'n' 
         Elapsed Time   elapsed_seconds  'n'
         Cols used   train_cols  'n';

    return best_score;
}



void run_tests() {
    stdcout  Best RMSE   feature_selection();
}

int main() {
    for (int i = 1; i  11 + 1; i++) feature_selection(i, 1000000000, false);
    return 0;
}
