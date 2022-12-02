# SLOP: Bodybuilding Diet Planning by Solving Linear Programs

Bodybuilders usually follow a high carb, high protein, low fat diet and need a minimum amount of calories to
grow their muscles. Determining what to eat while staying within the proper macronutrient ranges can be challenging.
Normal diets aren't amenable to linear programming since most people care about variety and what the food they're eating
tastes like. Since bodybuilding diets are more about performance than enjoyment they can be satisfactorily solved by
casting the diet planning problem as an LP problem.

To keep the problem simple I haven't added any constraints on micronutrients but extending this project to handle
that would be relatively straightforward.

## Dependencies

This code requires simdjson and google's or-tools. The Makefile will download these libraries when you run `make` for the
first time. This code will only build on OS X but getting it running on other platforms shouldn't be too hard.

## Building and Running

Run `make`. The first time you run `make` it will call `install_dependencies.sh`, which makes a local directory `ext/` and
downloads the required libraries there. Subsequent calls to `make` will not download anymore files. After the dependencies
are installed the code in src/main.cpp will be compiled and linked into a binary called `SLOP`.

`SLOP` reads two json files found in `data/`, `constraints.json` and `meals.json`, the format is straightforward. You can modify
those documents as desired. The `data/` directory must be in the same directory as `SLOP` or the program will not be able to find
the data files. `SLOP` parses those files and uses them to create 

## What Is Minimized

Something needs to be minimized in order for the LP solver to find a solution (otherwise there are probably infinitely many 
feasible solutions and it has no idea which one to pick). I would like to be able to minimize total food prep time, but this
is not a linear function since it depends on which variables are selected to be non-zero, not on their value (takes the same
amount of time to make two bagels as it does to make one bagel more or less). So instead I'm optimizing the sum of each variable
times a user definable value named "preference". For example, I prefer steak to chicken breast so steak gets a higher preference
value.

## Isn't This A Mixed Integer Programming Problem?

Yes and no. Some foods, like mashed potatoes, are easily divided up. Basically any meal can be divided up but cooking an arbitrary
quantity might be difficult. In practice I don't think this is a real problem because on a bodybuilding diet you will mostly be 
eating the same sorts of foods repeatedly and so any leftovers you can just save and eat later on, making more or less of that food
as needed the next time you're planning on having it.

## Possible Extensions

- Micronutrient constraints. This would be handled in the same way as calories and the macronutrients are handled. For example,
  someone with high blood pressure may want to eat less salt and so set a limit on how much salt can be consumed.
- Instead of planning one day at a time, plan one week or longer at a time. This allows more variety at the expense of greater
  complexity since you then have to figure out how to allocate the meals among the days. Another approach would be to plan
  day-by-day but to vary the constraints by day, which would allow more variety rather than getting the same plan every day.
- Convert to a mixed integer programming problem. This is really easy to do with Google's library. It just requires specifiying
  that some of the variables are integer values and maybe setting a timeout so it doesn't run too long if it can't find a solution.

