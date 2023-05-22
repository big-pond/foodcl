    SELECT days.id, days.mdate, 
	      (SELECT sum(meal.weight) FROM meal WHERE meal.days_id=days.id) AS mweight, 
          (SELECT sum(meal.weight*products.calory/100) FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id) AS ccal, 
          round((SELECT sum(meal.weight*products.protein/100) FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id),1) AS prot, 
          round((SELECT sum(meal.weight*products.fat/100) FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id),1) AS fats, 
          round((SELECT sum(meal.weight*products.carbohydrate/100) FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id),1) AS carb 
          FROM days ORDER BY days.mdate
