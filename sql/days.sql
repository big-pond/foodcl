    SELECT days.id, days.mdate, 
	      (SELECT sum(meal.weight) FROM meal WHERE meal.days_id=days.id) AS mweight, 
          (SELECT sum(meal.weight*products.calory/100) FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id) AS ccal, 
          (SELECT sum(meal.weight*products.protein/100) FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id) AS prot, 
          (SELECT sum(meal.weight*products.fat/100) FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id) AS fats, 
          (SELECT sum(meal.weight*products.carbohydrate/100) FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id) AS carb, 
          (SELECT weight FROM weight WHERE weight.days_id=days.id) AS pweight 
          FROM days ORDER BY days.mdate
