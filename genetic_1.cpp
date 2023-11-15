#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <numeric>

using namespace std;

const int POPULATION_SIZE = 100;
const double MUTATION_RATE = 0.1;
const int GENERATIONS = 100;

const vector<string> WORKOUT = { "Swimming", "Pilates", "Yoga", "Cardio", "Stretching", "CrossFit" };
const vector<string> ROOMS = { "Gym 1", "Gym 2", "Gym 3" };
const vector<string> GROUPS = { "Children", "Teen ", "Pregnant", "Adult Junior", "Adult Middle", "Adult Senior" };
const int WORKOUT_PER_DAY = 4;

struct Class {
    string name;
    string room;
    string group;
    int timeslot;
};

// Оцінка фітнес-функції для розкладу
int fitness(const vector<Class>& schedule) {
    int conflicts = 0;
    for (size_t i = 0; i < schedule.size(); ++i) {
        for (size_t j = i + 1; j < schedule.size(); ++j) {
            if (schedule[i].timeslot == schedule[j].timeslot && schedule[i].room == schedule[j].room) {
                conflicts += 1;
            }
        }
    }
    return conflicts;
}

// Мутація розкладу
vector<Class> mutate(const vector<Class>& schedule) {
    vector<Class> mutated_schedule = schedule;
    int random_class_index = rand() % mutated_schedule.size();
    mutated_schedule[random_class_index].timeslot = rand() % WORKOUT_PER_DAY;
    return mutated_schedule;
}

// Схрещування двох розкладів
vector<Class> crossover(const vector<Class>& parent1, const vector<Class>& parent2) {
    int split_point = rand() % (parent1.size() - 1) + 1;
    vector<Class> child(parent1.begin(), parent1.begin() + split_point);
    child.insert(child.end(), parent2.begin() + split_point, parent2.end());
    return child;
}

// Генетичний алгоритм
vector<Class> genetic_algorithm() {
    vector<vector<Class>> population(POPULATION_SIZE, vector<Class>());
    vector<int> fitness_scores;

    for (auto& schedule : population) {
        // Створення розкладу
        schedule.resize(GROUPS.size() * WORKOUT_PER_DAY);
        for (size_t i = 0; i < GROUPS.size(); ++i) {
            for (int j = 0; j < WORKOUT_PER_DAY; ++j) {
                schedule[i * WORKOUT_PER_DAY + j] = {
                    WORKOUT[rand() % WORKOUT.size()],
                    ROOMS[rand() % ROOMS.size()],
                    GROUPS[i],
                    0
                };
            }
        }

        // Оцінка фітнес-функції і додавання до вектора оцінок
        fitness_scores.push_back(fitness(schedule));
    }

    for (int generation = 0; generation < GENERATIONS; ++generation) {
        vector<size_t> sorted_indices(population.size());
        iota(sorted_indices.begin(), sorted_indices.end(), 0);
        sort(sorted_indices.begin(), sorted_indices.end(),
            [&fitness_scores](size_t i, size_t j) { return fitness_scores[i] < fitness_scores[j]; });

        vector<vector<Class>> selected_population;
        for (size_t i = 0; i < 0.5 * POPULATION_SIZE; ++i) {
            selected_population.push_back(population[sorted_indices[i]]);
        }

        vector<vector<Class>> new_population = selected_population;

        while (new_population.size() < static_cast<size_t>(POPULATION_SIZE)) {
            const auto& parent1 = selected_population[rand() % selected_population.size()];
            const auto& parent2 = selected_population[rand() % selected_population.size()];

            auto child = crossover(parent1, parent2);

            if (rand() / static_cast<double>(RAND_MAX) < MUTATION_RATE) {
                child = mutate(child);
            }

            new_population.push_back(child);
        }

        population = new_population;

        size_t best_schedule_index = min_element(fitness_scores.begin(), fitness_scores.end()) - fitness_scores.begin();
        int best_fitness = fitness_scores[best_schedule_index];
        cout << "Iteration " << generation + 1 << "/" << GENERATIONS << " - Best Fitness: " << best_fitness << endl;
    }

    size_t best_schedule_index = min_element(fitness_scores.begin(), fitness_scores.end()) - fitness_scores.begin();
    vector<Class> best_schedule = population[best_schedule_index];

    return best_schedule;
}

int main() {
    vector<Class> best_schedule = genetic_algorithm();

    cout << "\nBest Schedule:" << endl;
    for (const auto& class_obj : best_schedule) {
        cout << class_obj.group << " - " << class_obj.name << " - " << class_obj.room << " - Timeslot " << class_obj.timeslot << endl;
    }

    return 0;
}
