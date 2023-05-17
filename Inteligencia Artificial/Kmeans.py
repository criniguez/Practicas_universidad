import numpy as np
import utils
from scipy.spatial.distance import cdist


class KMeans:

    def __init__(self, X, K=1, options=None):
        """
         Constructor of KMeans class
             Args:
                 K (int): Number of cluster
                 options (dict): dictºionary with options
            """
        self.num_iter = 0
        self.K = K
        self._init_X(X)
        self._init_options(options)  # DICT options
        self._init_centroids()



    def _init_X(self, X):
        """Initialization of all pixels, sets X as an array of data in vector form (PxD)
            Args:
                X (list or np.array): list(matrix) of all pixel values
                    if matrix has more than 2 dimensions, the dimensionality of the smaple space is the length of
                    the last dimension
        """
        #######################################################
        ##  YOU MUST REMOVE THE REST OF THE CODE OF THIS FUNCTION
        ##  AND CHANGE FOR YOUR OWN CODE
        #######################################################
        if X.dtype != float:
            X.astype(np.float64)
        
        if len(X.shape) == 2: 
            self.X = X
        else: 
            self.X = np.reshape(X, (X.shape[0] * X.shape[1], X.shape[-1]))

    def _init_options(self, options=None):
        """
        Initialization of options in case some fields are left undefined
        Args:
            options (dict): dictionary with options
        """
        if options == None:
            options = {}
        
        if not 'km_init' in options:
            options['km_init'] = 'first'
        
        if not 'tolerance' in options:
            options['tolerance'] = 0
        
        if not 'max_iter' in options:
            options['max_iter'] = 1_000_000
        
        if not 'threshold' in options: 
            options['threshold'] = 20
        
        
        self.options = options
        self.threshold = 0
        self.num_iter = 0

    def _init_centroids(self):
        """
        Initialization of centroids
        """
        ret = []
        aux = self.X.tolist()
        if self.options['km_init'].lower() == 'first': 
            for i in aux: 
                if i not in ret: ret.append(i)
                if len(ret) == self.K: break
        
        elif self.options['km_init'].lower() == 'random':
            while True:
                i = aux[np.random.randint(self.X.shape[0])]
                if i not in ret: ret.append(i)
                if len(ret) == self.K: break
        
        elif self.options['km_init'].lower() == 'custom':
            aux2 = len(aux)//2
            for i in range(aux2):
                if aux[i] not in ret: ret.append(aux[i])
                if len(ret) == self.K: break
                if aux[aux2+i] not in ret: ret.append(aux[aux2+i])
                if len(ret) == self.K: break

        self.centroids = np.array(ret, dtype=np.float64)
        self.old_centroids = np.array(ret, dtype=np.float64)

    def get_labels(self):
        """        Calculates the closest centroid of all points in X
        and assigns each point to the closest centroid
        """
        self.labels = np.argmin(distance(self.X, self.centroids), axis=1)
    
    def get_centroids(self):
        """
        Calculates coordinates of centroids based on the coordinates of all the points assigned to the centroid
        """
        self.old_centroids = np.copy(self.centroids)
        self.centroids = np.array([np.mean(self.X[np.where(self.labels == i)], axis=0) for i in range(self.K)])

    def converges(self):
        """
        Checks if there is a difference between current and old centroids
        """
        return np.allclose(self.centroids, self.old_centroids, atol=self.options['tolerance'])

    def fit(self):
        """
        Runs K-Means algorithm until it converges or until the number
        of iterations is smaller than the maximum number of iterations.
        """
        for i in range(self.options['max_iter']):
            self.get_labels()
            self.get_centroids()
            if self.converges(): 
                self.num_iter += i
                break
       
    def whitinClassDistance(self):
        """
         returns the whithin class distance of the current clustering
        """
        return np.mean(np.array([j[self.labels[i]] for i, j in enumerate(distance(self.X, self.centroids))])**2)

    def find_bestK(self, max_K):
        """
         sets the best k anlysing the results up to 'max_K' clusters
        """
        l2 = []
        self.K = 2
        self._init_centroids()
        self.fit()
        old = self.whitinClassDistance()
        l2.append(old)
        for i in range(self.K+1, max_K+1):
            self.K = i
            self._init_centroids()
            self.fit()
            actual = self.whitinClassDistance()
            l2.append(actual)
            if 100 - 100 * actual/old < self.options['threshold']: 
                self.wcd = l2
                self.threshold = 100 - 100 * actual/old
                break
            else: old = actual
        
        self.K  -= 1

def distance(X, C):
    """
    Calculates the distance between each pixcel and each centroid
    Args:
        X (numpy array): PxD 1st set of data points (usually data points)
        C (numpy array): KxD 2nd set of data points (usually cluster centroids points)

    Returns:
        dist: PxK numpy array position ij is the distance between the
        i-th point of the first set an the j-th point of the second set
    """
    return cdist(X, C, "euclidean")


def get_colors(centroids):
    """
    for each row of the numpy matrix 'centroids' returns the color laber folllowing the 11 basic colors as a LIST
    Args:
        centroids (numpy array): KxD 1st set of data points (usually centroind points)

    Returns:
        lables: list of K labels corresponding to one of the 11 basic colors
    """
    return utils.colors[np.argmax(utils.get_color_prob(centroids), axis=1)]