SELECT meal.id, meal.mtime, meal.product_id,
                  products.name, meal.weight, products.calory, 
                  round(products.calory*meal.weight/100.0, 1) as ccal, 
                  round(products.protein*meal.weight/100, 1) as prot, 
                  round(products.fat*meal.weight/100, 1) as fat, 
                  round(products.carbohydrate*meal.weight/100, 1) as carb 
                  FROM meal, products WHERE meal.product_id=products.id AND meal.days_id=5 
                  ORDER BY meal.mtime
