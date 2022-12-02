#include <iostream>
#include <memory>
#include <vector>

#include "ortools/linear_solver/linear_solver.h"
#include "simdjson.h"

using namespace operations_research;

const int CALS_CARB = 4;
const int CALS_PROTEIN = 4;
const int CALS_FAT = 9;
const int CALS_ALCOHOL = 7;
const double CALS_FIBER = 1.5;

struct Data {
    MPVariable* v;
    double pref;
    double fat;
    double protein;
    double carbs;
    double fiber;
    double alcohol;
};

void solve(simdjson::ondemand::document& data) {
    // Get calorie and macronutrient constraints
    auto info = data["info"].get_object();
    double bodyweight = info["bodyweight_lb"].get_double();

    double min_cals = bodyweight * info["min_cal_per_lb"].get_double();
    double max_cals = bodyweight * info["max_cal_per_lb"].get_double();

    double min_protein = bodyweight * info["min_protein_per_lb"].get_double();
    double max_protein = bodyweight * info["max_protein_per_lb"].get_double();

    double min_carbs = bodyweight * info["min_carbs_per_lb"].get_double();
    double max_carbs = bodyweight * info["max_carbs_per_lb"].get_double();

    double min_fat = bodyweight * info["min_fat_per_lb"].get_double();
    double max_fat = bodyweight * info["max_fat_per_lb"].get_double();

    std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("GLOP"));
    if (!solver) {
        LOG(WARNING) << "GLOP solver unavailable";
        return;
    }

    std::vector<Data> constraint_data;

    auto meals = data["meals"].get_array();

    // create variables for LP problem
    for (auto meal : meals) {
        Data d;
        d.pref = meal["preference"].get_double();
        d.protein = meal["grams_protein"].get_double();
        d.carbs = meal["grams_carbs"].get_double();
        d.fat = meal["grams_fat"].get_double();
        d.fiber = meal["grams_fiber"].get_double();
        d.alcohol = meal["grams_alcohol"].get_double();
        double max_servings = meal["max_servings"].get_double();

        std::string_view name = meal["name"].get_string();
        std::string tmp_name = static_cast<std::string>(name);
        if (meal["required"].get_bool()) {
            d.v = solver->MakeNumVar(1.0, max_servings, tmp_name);
        } else {
            d.v = solver->MakeNumVar(0.0, max_servings, tmp_name);
        }
        constraint_data.push_back(d);
    }

    // add constraints + objective to LP problem
    MPConstraint* const calories = solver->MakeRowConstraint(min_cals, max_cals);
    MPConstraint* const protein = solver->MakeRowConstraint(min_protein, max_protein);
    MPConstraint* const carbs = solver->MakeRowConstraint(min_carbs, max_carbs);
    MPConstraint* const fat = solver->MakeRowConstraint(min_fat, max_fat);
    MPObjective* const objective = solver->MutableObjective();
    objective->SetMaximization();
    for (auto& d : constraint_data) {
        calories->SetCoefficient(d.v,
                                 CALS_CARB * d.carbs + 
                                    CALS_PROTEIN * d.protein + 
                                    CALS_FAT * d.fat +
                                    CALS_FIBER * d.fiber +
                                    CALS_ALCOHOL * d.alcohol
                                );
        protein->SetCoefficient(d.v, d.protein);
        carbs->SetCoefficient(d.v, d.carbs);
        fat->SetCoefficient(d.v, d.fat);

        objective->SetCoefficient(d.v, d.pref);
    }

    const MPSolver::ResultStatus result_status = solver->Solve();
    if (result_status != MPSolver::OPTIMAL) {
        std::cout << "The problem does not have an optimal solution!" << std::endl;
        return;
    }

    double total_cals = 0;
    double total_fat = 0;
    double total_protein = 0;
    double total_carbs = 0;
    std::cout << "Optimal objective value = " << objective->Value() << std::endl;
    for (auto& d : constraint_data) {
        double weight = d.v->solution_value();
        if (weight != 0) {
            std::cout << d.v->name() << " = " << weight << std::endl;
            total_fat += weight * d.fat;
            total_carbs += weight * d.carbs;
            total_protein += weight * d.protein;

            total_cals += weight * (d.fat * CALS_FAT + d.protein * CALS_PROTEIN + d.carbs * CALS_CARB);
        }
    }

    std::cout << std::endl;
    std::cout << "Calories: " << min_cals << " <= " << total_cals << " <= " << max_cals << std::endl;
    std::cout << "Carbs: " << min_carbs << " <= " << total_carbs << " <= " << max_carbs << std::endl;
    std::cout << "Fat: " << min_fat << " <= " << total_fat << " <= " << max_fat << std::endl;
    std::cout << "Protein " << min_protein << " <= " << total_protein << " <= " << max_protein << std::endl;
}

int main() {
  simdjson::ondemand::parser parser;
  simdjson::padded_string json = simdjson::padded_string::load("data/data.json");
  simdjson::ondemand::document data = parser.iterate(json);

  solve(data);

  return EXIT_SUCCESS;
}
