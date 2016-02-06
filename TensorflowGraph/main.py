import numpy as np
import tempfile
import tensorflow as tf

from tf_rl.controller import DiscreteDeepQ
#from tf_rl.simulation import KarpathyGame
from tf_rl import simulate
from tf_rl.models import MLP

tf.ops.reset_default_graph()
session = tf.Session()

# This little guy will let us run tensorboard
#      tensorboard --logdir [LOG_DIR]
journalist = tf.train.SummaryWriter("/Users/anton/devel/unity/QuadrocopterHabr/TensorflowLog")

# Brain maps from observation to Q values for different actions.
# Here it is a done using a multi layer perceptron with 2 hidden
# layers

# observation
# double currentRotW,
# double currentRotX,
# double currentRotY,
# double currentRotZ,
#
# double targetX,
# double targetY,
# double targetZ,
#
# double motor1powerVal,
# double motor2powerVal,
# double motor3powerVal,
# double motor4powerVal
observation_size = 11;

# actions
# increase, decrease
# 2 actions per motor
num_actions = 8;

brain = MLP([observation_size,], [40, 40, num_actions], 
            [tf.tanh, tf.tanh, tf.identity])

# The optimizer to use. Here we use RMSProp as recommended
# by the publication
optimizer = tf.train.RMSPropOptimizer(learning_rate= 0.0001, decay=0.9)

# DiscreteDeepQ object
current_controller = DiscreteDeepQ(observation_size, num_actions, brain, optimizer, session, discount_rate=0.9, exploration_period=5000, max_experience=10000, store_every_nth=4, train_every_nth=4, summary_writer=journalist)

init_all_vars_op = tf.initialize_variables(tf.all_variables(), name='init_all_vars_op')

session.run(tf.initialize_all_variables())

tf.train.write_graph(session.graph_def, 'models/', 'graph.pb', as_text=False)
