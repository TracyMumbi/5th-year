class CreateParameters < ActiveRecord::Migration[7.1]
  def change
    create_table :parameters do |t|
      t.integer :speed
      t.integer :distance

      t.timestamps
    end
  end
end
